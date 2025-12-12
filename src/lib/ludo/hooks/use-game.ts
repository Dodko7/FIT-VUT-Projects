import { useParams, useRouter, useSearchParams } from "next/navigation";
import type {
	DiceRoll,
	FullGame,
	LudoGameState,
	PlayerGameState,
} from "../types";
import { Color } from "@prisma/client";
import { useQuery, useQueryClient } from "@tanstack/react-query";
import { LoadGameByName } from "../client-api/load";
import { useMemo, useState } from "react";
import LudoClientState from "../client-state";

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

	// Selected pawn
	const [selectedPawnId, setSelectedPawnId] = useState<number>(-1);

	// Avaliable moves
	const [avaliableMoves, setAvaliableMoves] = useState<number[]>([]);

	// Client state
	const [clientState, setClientState] = useState(LudoClientState.AWAITING_PLAYER_MOVE);

	/**
	 * EVENT HANDLERS
	 */

	// Roll dice handler
	const onRollDice = async (): Promise<void> => {
		await fetch(`/api/ludo/${gameName}/roll`, {
			method: "POST",
			headers: {
				"Content-Type": "application/json",
			},
		});
	};

	// Select pawn handler
	const onSelectPawn = async (pawnId: number): Promise<void> => {
		setSelectedPawnId(pawnId);

		// TODO API CALL
		await Promise.resolve();
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
			isLoading,
			error,

			// Game state
			state: clientState,
			diceNumber: game?.diceRoll as DiceRoll,
			currentTurn: game?.turn || Color.RED,
			isPaused: isPaused,

			// Entities
			players: players,
			selectedPawnId: selectedPawnId,
			name: game?.name || "",
			avaliableMoves: [], // todo
			avaliablePawns: [], // todo

			// Handlers
			onRollDice,
			onSelectPawn,
			onMovePawn,
			onPauseGame,
			onResumeGame,
			onQuitGame,
		};
	}, [game, isLoading, error, clientState, isPaused, selectedPawnId]);

	return state;
}
