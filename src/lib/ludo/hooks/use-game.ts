import { useParams, useRouter, useSearchParams } from "next/navigation";
import { useEffect } from "react";
import type {
	DiceRoll,
	FullGame,
	LudoGameState,
	PlayerGameState,
	Result,
} from "../types";
import { Color } from "@prisma/client";
import useSocket from "./use-socket";
import { useQuery, useQueryClient } from "@tanstack/react-query";
import { LoadGameByName } from "../client-api/load";
import { useMemo, useState } from "react";
import { LudoClientState } from "../client-state";
import { LeaveGame } from "../client-api/leave";

/**
 * Custom hook to access the current Ludo game state.
 * @returns The current Ludo game state or null if not available.
 */
export default function useGame(): LudoGameState {
	// Page URL
	const params = useParams();
	const searchParams = useSearchParams();

	const gameName = params.gameName as string;
	const color = searchParams.get("color") as Color | null;
	const preferredColor = searchParams.get("preferredColor") as Color | null;

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

	// On socket game update
	useEffect(() => {
		if (!socket) return;
		socket.emit("join", `game_${gameName}`);

		socket.on("game-update", invalidate);

		// Cleanup on unmount or socket change
		return () => {
			socket.off("game-update", invalidate);
		};
	}, [socket, gameName]);

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

	// Quit game handler
	const onQuitGame = async (): Promise<void> => {
		if (!color) {
			// TODO
			console.error("No color specified for quitting the game.");
			return;
		}

		await LeaveGame(gameName, color);
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
			onQuitGame,
		};
	}, [game, isLoading, error, clientState, isPaused, selectedPawnId]);

	return state;
}
