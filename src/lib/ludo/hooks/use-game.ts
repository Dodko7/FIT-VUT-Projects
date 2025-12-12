import { useParams, useSearchParams } from "next/navigation";
import type {
	DiceRoll,
	FullGame,
	LudoGameState,
	PlayerGameState,
} from "../types";
import { Color } from "@prisma/client";
import useSocket from "./use-socket";
import { useQuery } from "@tanstack/react-query";
import { LoadGameById } from "../client-api/load";
import { useMemo, useState } from "react";
import { LudoClientState } from "../client-state";

/**
 * Custom hook to access the current Ludo game state.
 * @returns The current Ludo game state or null if not available.
 */
export default function useGame(): LudoGameState {
	// Page URL
	const params = useParams();
	const searchParams = useSearchParams();

	const id = Number(params.gameId);
	const color = searchParams.get("color") as Color | null;

	// Fetch game model
	const {
		data: game,
		isLoading,
		error,
		refetch,
	} = useQuery<FullGame>({
		queryKey: ["ludo", "game", id],
		queryFn: () => LoadGameById(id),
		enabled: !!params.gameId,
	});

	const name = game?.name || "";

	// Websockets connection
	const { socket, connectionError } = useSocket(name);

	// Is game paused?
	const [isPaused, setIsPaused] = useState(false);

	// Selected pawn
	const [selectedPawnId, setSelectedPawnId] = useState<number>(-1);

	// Avaliable moves
	const [avaliableMoves, setAvaliableMoves] = useState<number[]>([]);

	// Game "visual" state
	const [clientState, setClientState] = useState<LudoClientState>(
		game?.turn === searchParams.get("color") ?
			LudoClientState.TURN_ROLL
		:	LudoClientState.WAITING,
	);

	/**
	 * EVENT HANDLERS
	 * TODO: Add socket emitters here
	 */

	// Roll dice handler
	const onRollDice = async (): Promise<void> => {
		await fetch(`/api/ludo/${id}/roll`, {
			method: "POST",
			headers: {
				"Content-Type": "application/json",
			},
			body: JSON.stringify({ color }),
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
			connectionError,

			// Game state
			state: clientState,
			diceNumber: game?.diceRoll as DiceRoll,
			currentTurn: game?.turn || Color.RED,
			isPaused: isPaused,
			clientColor: color,

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
		};
	}, [game, isLoading, error, clientState, isPaused, selectedPawnId]);

	return state;
}
