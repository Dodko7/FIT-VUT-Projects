"use client";

import { Color } from "@prisma/client";
import { useQuery } from "@tanstack/react-query";
import { useState } from "react";
import BoardNavbar from "~/components/games/ludo/board/board-navbar";
import HorizontalBoardPart from "~/components/games/ludo/board/horizontal-board-part";
import Middle from "~/components/games/ludo/board/middle";
import PlayerStart from "~/components/games/ludo/board/player-start";
import PlayerStatus from "~/components/games/ludo/board/player-status";
import VerticalBoardPart from "~/components/games/ludo/board/vertical-board-part";
import LudoErrorPage from "~/components/games/ludo/pages/error-page";
import LudoLoadingPage from "~/components/games/ludo/pages/loading-page";
import LudoPausePage from "~/components/games/ludo/pages/pause-page";
import {
	BLUE_PAWN_HOME_PROPS,
	BOARD_BOTTOM_PART,
	BOARD_LEFT_PART,
	BOARD_RIGHT_PART,
	BOARD_TOP_PART,
	GREEN_PAWN_HOME_PROPS,
	RED_PAWN_HOME_PROPS,
	YELLOW_PAWN_HOME_PROPS,
} from "~/lib/ludo/constants";
import type { FullGame, PawnPosition } from "~/lib/ludo/types";
import {
	CompletedPawnsFromColor,
	GetPawnSpotPropsForBoardPart,
	PawnsInStartFromPlayer,
} from "~/lib/ludo/utils";
import { useParams, useSearchParams } from "next/navigation";
import { LoadGameById } from "~/lib/ludo/client-api/load";
import { useEffect } from "react";
import { io, type Socket } from "socket.io-client";

let socket: Socket;

export default function LudoGameplayPage() {
    const params = useParams();
    const searchParams = useSearchParams();
    const gameId = Number(params.gameId);
    const myColor = searchParams.get("color") as Color | null;

	// Fetch game state
	const {
		data: game,
		isLoading,
		error,
		refetch,
	} = useQuery<FullGame>({
		queryKey: ["ludo", "game", gameId],
		queryFn: () => LoadGameById(gameId),
		enabled: !!gameId,
	});

    const [connectionError, setConnectionError] = useState<string | null>(null);

	useEffect(() => {
		socket = io();
		socket.on("connect", () => {
			console.log("Connected to socket server", socket.id);
            setConnectionError(null);
			socket.emit("join-game", gameId.toString());
		});
        socket.on("connect_error", (err) => {
            console.error("Socket connection error:", err);
            setConnectionError("Failed to connect to game server. Please check your connection.");
        });
		socket.on("game-updated", () => {
			console.log("Game updated event received!");
			void refetch();
		});
		return () => {
			if (socket) socket.disconnect();
		};
	}, [gameId, refetch]);

	// Is the game paused?
	const [isPaused, setIsPaused] = useState<boolean>(false);

	// Avaliable pawns to move
	const [avaliablePawns, setAvailablePawns] = useState<number[]>([]);

	// Selected pawn
	const [selectedPawnId, setSelectedPawnId] = useState<number | null>(null);

	// Available moves for the selected pawn
	const [avaliableMoves, setAvailableMoves] = useState<number[]>([]);

	// Is the dice rolling?
	const [isRolling, setIsRolling] = useState(false);

	const handleRollDice = () => {
        // Only allow rolling if it's my turn
        if (game?.turn !== myColor) return;
		if (isRolling) return;
		setIsRolling(true);

		// Simulate rolling time (replace with API call later)
		setTimeout(() => {
			const newRoll = Math.floor(Math.random() * 6) + 1;
			// setDiceRoll(newRoll); // Removed local state
			setIsRolling(false);
            // TODO: Call API to save roll
            socket.emit("game-action", gameId.toString());
		}, 1000);
	};

	if (isLoading) {
		return <LudoLoadingPage />;
	} else if (connectionError) {
        return (
            <LudoErrorPage
                message={connectionError}
                links={[{ text: "Back to Menu", link: "/games/ludo/menu" }]}
            />
        );
    } else if (error || !game) {
		return (
			<LudoErrorPage
				message={error?.message || "Failed to load current game."}
			/>
		);
	} else if (isPaused) {
		return <LudoPausePage />;
	}

	// Number of players (useful)
	const numberOfPlayers = game.players.length;

	// Individual players (useful)
	const redPlayer = game.players[0]!;
	const greenPlayer = game.players[1]!;

	// These won't always be there
	const bluePlayer = numberOfPlayers >= 3 ? game.players[2] : null;
	const yellowPlayer = numberOfPlayers === 4 ? game.players[3] : null;

	// TODO move this to the backend
	const positions: PawnPosition[] = game.players.flatMap(
		(player): PawnPosition[] => {
			return player.pawns.map((pawn) => ({
				position: pawn.position,
				color: player.color,
			}));
		},
	);

	const topPartProps = GetPawnSpotPropsForBoardPart(
		BOARD_TOP_PART,
		positions,
	);
	const rightPartProps = GetPawnSpotPropsForBoardPart(
		BOARD_RIGHT_PART,
		positions,
	);
	const bottomPartProps = GetPawnSpotPropsForBoardPart(
		BOARD_BOTTOM_PART,
		positions,
	);
	const leftPartProps = GetPawnSpotPropsForBoardPart(
		BOARD_LEFT_PART,
		positions,
	);

	return (
		<div className="ludo-board-font flex h-screen w-screen flex-col items-center justify-start">
			{/** Top navbar */}
			<BoardNavbar
				gameName={game.name}
				onPause={() => setIsPaused(true)}
			/>
			{/** Red and green player statuses */}
			<div className="flex w-full">
				<PlayerStatus
					hasContent={true}
					playerName={redPlayer.name}
					completedPawns={CompletedPawnsFromColor(redPlayer)}
					border={true}
					className="ludo-red-area"
				/>
				<PlayerStatus
					hasContent={true}
					playerName={greenPlayer.name}
					completedPawns={CompletedPawnsFromColor(greenPlayer)}
					border={false}
					className="ludo-green-area"
				/>
			</div>
			{/** BOARD CONTENT */}
			<div className="ludo-board grid h-full w-full grid-cols-3 grid-rows-3">
				{/** Red player home */}
				<PlayerStart
					divClassName="ludo-red-area border-r-4"
					pawnSpotClassName="ludo-red-circle"
					pawnsPresent={PawnsInStartFromPlayer(redPlayer)}
					pawnProps={RED_PAWN_HOME_PROPS}
				/>
				{/** Vertical board part between red and green */}
				<VerticalBoardPart
					extraClassNames="border-t-4 border-black"
					pawnSpotProps={topPartProps.pawnSpotProps}
				/>
				{/** Green player home */}
				<PlayerStart
					divClassName="ludo-green-area border-l-4"
					pawnSpotClassName="ludo-green-circle"
					pawnsPresent={PawnsInStartFromPlayer(greenPlayer)}
					pawnProps={GREEN_PAWN_HOME_PROPS}
				/>
				{/** Horizontal board part between red and blue */}
				<HorizontalBoardPart
					pawnSpotProps={leftPartProps.pawnSpotProps}
				/>
				{/** Center board area */}
				<Middle
					diceNumber={game.diceRoll ?? 6}
					playerColor={game.turn}
					playerName={
						game.players.find((p) => p.color === game.turn)
							?.name || ""
					}
					onRollDice={handleRollDice}
					isRolling={isRolling}
				/>
                {/* Turn Indicator Overlay */}
                {game.turn !== myColor && (
                    <div className="absolute top-20 left-1/2 -translate-x-1/2 rounded-full bg-black/50 px-6 py-2 text-white backdrop-blur-sm">
                        Waiting for {game.turn}...
                    </div>
                )}
				{/** Horizontal board part between green and yellow */}
				<HorizontalBoardPart
					pawnSpotProps={rightPartProps.pawnSpotProps}
				/>
				{/** Blue player home */}
				<PlayerStart
					divClassName="ludo-blue-area border-r-4"
					pawnSpotClassName="ludo-blue-circle"
					pawnsPresent={
						bluePlayer ?
							PawnsInStartFromPlayer(bluePlayer)
						:	[false, false, false, false]
					}
					pawnProps={BLUE_PAWN_HOME_PROPS}
				/>
				{/** Vertical board part between yellow and blue */}
				<VerticalBoardPart
					pawnSpotProps={bottomPartProps.pawnSpotProps}
					extraClassNames="border-b-4 border-black"
				/>
				{/** Yellow player home */}
				<PlayerStart
					divClassName="ludo-yellow-area border-l-4"
					pawnSpotClassName="ludo-yellow-circle"
					pawnsPresent={
						yellowPlayer ?
							PawnsInStartFromPlayer(yellowPlayer)
						:	[false, false, false, false]
					}
					pawnProps={YELLOW_PAWN_HOME_PROPS}
				/>
			</div>
			{/** Blue and yellow player statuses */}
			<div className="flex w-full border-t">
				<PlayerStatus
					hasContent={true}
					playerName={bluePlayer?.name || ""}
					completedPawns={
						bluePlayer ? CompletedPawnsFromColor(bluePlayer) : 0
					}
					border={true}
					className="ludo-blue-area"
				/>
				<PlayerStatus
					hasContent={true}
					playerName={yellowPlayer?.name || ""}
					completedPawns={
						yellowPlayer ? CompletedPawnsFromColor(yellowPlayer) : 0
					}
					border={false}
					className="ludo-yellow-area"
				/>
			</div>
		</div>
	);
}
