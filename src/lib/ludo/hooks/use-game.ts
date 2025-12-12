import { useParams, useRouter, useSearchParams } from "next/navigation";
import type {
	HighlightedPawnSpot,
	AvaliablePawnMoves,
	DiceRoll,
	FullGame,
	LudoGameState,
	PlayerGameState,
	PawnSpotOnClicks,
} from "../types";
import { Color } from "@prisma/client";
import { useQuery, useQueryClient } from "@tanstack/react-query";
import { LoadGameByName } from "../client-api/load";
import { useMemo, useRef, useState } from "react";
import LudoClientState from "../client-state";
import { RollDice } from "../client-api/roll";
import PawnSpotHighlight from "../enum/pawn-spot-highlight";
import { get } from "http";
import { GetPawnIdAtPosition } from "../utils";
import MovePawn from "../client-api/move";

/**
 * Custom hook to access the current Ludo game state.
 * @returns The current Ludo game state or null if not available.
 */
export default function useGame(): LudoGameState {
	// Page URL
	const params = useParams();
	const searchParams = useSearchParams();

	const gameName = params.gameName as string;

	// For redirects
	const router = useRouter();

	// For refetching on updates
	const queryClient = useQueryClient();

	// Fetch game model
	const {
		data: game,
		isLoading,
		error,
	} = useQuery<FullGame>({
		queryKey: ["ludo", "game", gameName],
		queryFn: () => LoadGameByName(gameName),
		enabled: !!params.gameName,
	});

	// For convenience
	const invalidate = () => {
		queryClient.invalidateQueries({ queryKey: ["ludo", "game", gameName] });
	};

	const name = game?.name || "";

	// Is game paused?
	const [isPaused, setIsPaused] = useState(false);

	// Global/irreversible error
	const [globalError, setGlobalError] = useState<string | null>(null);

	// Selected pawn
	const [selectedPawnId, setSelectedPawnId] = useState<number | null>(null);

	// Avaliable moves
	const avaliableMoves = useRef<AvaliablePawnMoves[]>([]);

	// Avaliable pawns
	const avaliablePawns = useRef<HighlightedPawnSpot[]>([]);

	// Highlights
	const [highlights, setHighlights] = useState<HighlightedPawnSpot[]>([]);

	// On clicks
	const [onClicks, setOnClicks] = useState<PawnSpotOnClicks[]>([]);

	// Dice roll
	const [diceRoll, setDiceRoll] = useState<DiceRoll>(6);

	// Client state
	const [clientState, setClientState] = useState(
		LudoClientState.AWAITING_PLAYER_MOVE,
	);

	/**
	 * HELPERS
	 */
	const GetMoveSelectionHighlights = (id: number): HighlightedPawnSpot[] => {
		const moveHighlights = avaliableMoves.current.map((avalMove) => {
			if (avalMove.pawnId !== id) {
				return null;
			}
			return avalMove.moves;
		});

		return moveHighlights
			.flat()
			.filter((h): h is HighlightedPawnSpot => h !== null);
	};

	const GetPawnSelectionOnClicks = (
		spots: HighlightedPawnSpot[],
	): PawnSpotOnClicks[] => {
		return spots.map((spot): PawnSpotOnClicks => {
			return {
				position: spot.position,
				onClick: () => {
					const pawnId = GetPawnIdAtPosition(
						game!.players,
						spot.position,
					);
					setSelectedPawnId(pawnId);
					const newHighlights = GetMoveSelectionHighlights(pawnId!);
					const newOnClicks = GetMoveSelectionOnClicks(
						newHighlights,
						pawnId!,
					);
					setClientState(LudoClientState.AWAITING_PLAYER_MOVE);
					setHighlights(newHighlights);
					setOnClicks(newOnClicks);
				},
			};
		});
	};

	// Posts to /move to move a pawn
	const GetMoveSelectionOnClicks = (
		spots: HighlightedPawnSpot[],
		pawnId: number,
	): PawnSpotOnClicks[] => {
		return spots.map((spot): PawnSpotOnClicks => {
			return {
				position: spot.position,
				onClick: async () => {
					const res = await MovePawn(gameName, pawnId, spot.position);

					if (res.success) {
						invalidate();
						setSelectedPawnId(null);
						setHighlights([]);
						setOnClicks([]);
						avaliableMoves.current = [];
						avaliablePawns.current = [];
						setClientState(LudoClientState.AWAITING_PLAYER_MOVE);
					} else {
						setGlobalError(
							res.error ||
								"An unknown error occurred while moving the pawn.",
						);
					}
				},
			};
		});
	};

	/**
	 * EVENT HANDLERS
	 */

	// Roll dice handler
	const onRollDice = async (): Promise<void> => {
		setClientState(LudoClientState.DICE_ROLLING);
		const result = await RollDice(gameName);
		if (result.success) {
			const spots = result.value.avaliablePawns;
			const onClicks = GetPawnSelectionOnClicks(spots);
			setHighlights(spots);
			// Set refs and dice roll
			setOnClicks(onClicks);
			setDiceRoll(result.value.diceNumber);
			avaliableMoves.current = result.value.avaliableMoves;

			// -- DEBUG EVERYTHING --
			// console.log("Avaliable moves:", avaliableMoves.current);
			// console.log("Avaliable pawns:", result.value.avaliablePawns);
			// console.log("Dice roll:", result.value.diceNumber);
			// console.log("Highlights:", highlights);

			// // Set on clicks
			// console.log("OnClicks:", onClicks);
		}
		invalidate();
		setTimeout(() => {
			setClientState(LudoClientState.AWAITING_PAWN_SELECTION);
		}, 500);
	};

	// Move pawn handler
	const onMovePawn = async (
		pawnId: number,
		moveBy: number,
	): Promise<void> => {
		// TODO API CALL
		await Promise.resolve();
	};

	// Pause game handler
	const onPauseGame = (): void => {
		setIsPaused(true);
	};

	// Resume game handler
	const onResumeGame = (): void => {
		setIsPaused(false);
	};

	// Quit game handler
	const onQuitGame = (): void => {
		router.push("/games/ludo/menu");
	};

	// Compute the final state
	const state: LudoGameState = useMemo(() => {
		// Game players
		const players: PlayerGameState[] =
			game?.players.map((p) => ({
				name: p.name,
				color: p.color,
				pawns: p.pawns.map((pawn) => ({
					id: pawn.id,
					position: pawn.position,
					inHome: pawn.inHome,
					color: p.color,
				})),
			})) || [];

		return {
			// Connectivity
			isLoading: isLoading && !game,
			error,

			// Game state
			state: clientState,
			diceNumber: diceRoll,
			currentTurn: game?.turn || Color.RED,
			isPaused: isPaused,
			highlights,
			onClicks,

			// Entities
			players: players,
			selectedPawnId: selectedPawnId,
			name: game?.name || "",

			// Handlers
			onRollDice,
			onMovePawn,
			onPauseGame,
			onResumeGame,
			onQuitGame,
		};
	}, [game, isLoading, error, clientState, isPaused, selectedPawnId]);

	return state;
}
