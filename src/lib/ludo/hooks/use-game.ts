import { useParams, useSearchParams } from "next/navigation";
import type {
	DiceRoll,
	FullGame,
	LudoGameState,
	PlayerGameState,
} from "../types";
import type { Color } from "@prisma/client";
import useSocket from "./use-socket";
import { useQuery } from "@tanstack/react-query";
import { LoadGameById } from "../client-api/load";
import { useMemo, useState } from "react";
import LudoClientState from "../client-state";

/**
 * Custom hook to access the current Ludo game state.
 * @returns The current Ludo game state or null if not available.
 */
export default function useGame(): LudoGameState | null {
	// Page URL
	const params = useParams();
	const searchParams = useSearchParams();

	// Fetch game model
	const {
		data: game,
		isLoading,
		error,
		refetch,
	} = useQuery<FullGame>({
		queryKey: ["ludo", "game", Number(params.gameId)],
		queryFn: () => LoadGameById(Number(params.gameId)),
		enabled: !!params.gameId,
	});

    const id = Number(params.gameId);
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
			body: JSON.stringify({ color: searchParams.get("color") }),
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

	// Compute the final state
	const state: LudoGameState | null = useMemo(() => {
		if (!game || error) {
			return null;
		}

		// Game players
		const players: PlayerGameState[] = game.players.map((p) => ({
			name: p.name,
			color: p.color,
			pawns: p.pawns.map((pawn) => ({
				id: pawn.id,
				position: pawn.position,
				color: p.color,
			})),
		}));

		return {
			// Game state
			state: clientState,
			diceNumber: game.diceRoll as DiceRoll,
			currentTurn: game.turn,
			isPaused: isPaused,

			// Entities
			players: players,
			selectedPawnId: selectedPawnId,
			avaliableMoves: [], // todo
			avaliablePawns: [], // todo

			// Handlers
			onRollDice,
			onSelectPawn,
			onMovePawn,
		};
	}, [game, isLoading, error, clientState, isPaused, selectedPawnId]);

	return state;
}
