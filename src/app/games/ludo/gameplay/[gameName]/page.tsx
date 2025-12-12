"use client";

import BoardNavbar from "~/components/games/ludo/board/board-navbar";
import HorizontalBoardPart from "~/components/games/ludo/board/horizontal-board-part";
import Middle from "~/components/games/ludo/board/middle";
import PlayerStart from "~/components/games/ludo/board/player-start";
import PlayerStatus from "~/components/games/ludo/board/player-status";
import VerticalBoardPart from "~/components/games/ludo/board/vertical-board-part";
import LudoErrorPage from "~/components/games/ludo/pages/error-page";
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
import type { PawnGameState, PawnPosition } from "~/lib/ludo/types";
import {
	CompletedPawnsFromColor,
	GetPawnSpotPropsForBoardPart,
	PawnsInStartFromPlayer,
} from "~/lib/ludo/utils";
import useGame from "~/lib/ludo/hooks/use-game";
import { LudoClientState } from "~/lib/ludo/client-state";
import { Color } from "@prisma/client";
import LudoPausePage from "~/components/games/ludo/pages/pause-page";
import LudoLoadingPage from "~/components/games/ludo/pages/loading-page";

export default function LudoGameplayPage() {
	// Fetch state
	const game = useGame();

	// Errors
	if (game.error) {
		return (
			<LudoErrorPage
				message={game.error.message || "Failed to load current game."}
			/>
		);
	} else if (game.isLoading) {
		return <LudoLoadingPage />;
	} else if (game.isPaused) {
		return (
			<LudoPausePage
				onResume={game.onResumeGame}
				onQuit={game.onQuitGame}
				onExport={async () => {
					await Promise.resolve();
					return { success: true };
				}}
			/>
		);
	}

	console.log(game.players);

	// Individual players (useful)
	const redPlayer = game.players.find((p) => p.color === Color.RED);
	const greenPlayer = game.players.find((p) => p.color === Color.GREEN);
	const bluePlayer = game.players.find((p) => p.color === Color.BLUE);
	const yellowPlayer = game.players.find((p) => p.color === Color.YELLOW);

	// Pawns
	const pawns: PawnGameState[] = game.players.flatMap((p) => p.pawns);

	const topPartProps = GetPawnSpotPropsForBoardPart(
		BOARD_TOP_PART,
		pawns,
		game.highlights,
		game.onClicks,
	);
	const rightPartProps = GetPawnSpotPropsForBoardPart(
		BOARD_RIGHT_PART,
		pawns,
		game.highlights,
		game.onClicks,
	);
	const bottomPartProps = GetPawnSpotPropsForBoardPart(
		BOARD_BOTTOM_PART,
		pawns,
		game.highlights,
		game.onClicks,
	);
	const leftPartProps = GetPawnSpotPropsForBoardPart(
		BOARD_LEFT_PART,
		pawns,
		game.highlights,
		game.onClicks,
	);

	return (
		<div className="ludo-board-font flex h-screen w-screen flex-col items-center justify-start">
			{/** Top navbar */}
			<BoardNavbar
				gameName={game.name}
				onPause={game.onPauseGame}
			/>
			{/** Red and green player statuses */}
			<div className="flex w-full">
				<PlayerStatus
					hasContent={true}
					playerName={redPlayer?.name || ""}
					completedPawns={
						redPlayer ? CompletedPawnsFromColor(redPlayer) : 0
					}
					border={true}
					className="ludo-red-area"
				/>
				<PlayerStatus
					hasContent={true}
					playerName={greenPlayer?.name || ""}
					completedPawns={
						greenPlayer ? CompletedPawnsFromColor(greenPlayer) : 0
					}
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
					pawnsPresent={
						redPlayer ?
							PawnsInStartFromPlayer(redPlayer)
						:	[false, false, false, false]
					}
					pawnProps={RED_PAWN_HOME_PROPS}
					highlights={game.highlights}
					onClicks={game.onClicks}
					color={Color.RED}
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
					pawnsPresent={
						greenPlayer ?
							PawnsInStartFromPlayer(greenPlayer)
						:	[false, false, false, false]
					}
					pawnProps={GREEN_PAWN_HOME_PROPS}
					highlights={game.highlights}
					onClicks={game.onClicks}
					color={Color.GREEN}
				/>
				{/** Horizontal board part between red and blue */}
				<HorizontalBoardPart
					pawnSpotProps={leftPartProps.pawnSpotProps}
				/>
				{/** Center board area */}
				<Middle
					diceNumber={game.diceNumber ?? 6}
					playerColor={game.currentTurn}
					playerName={
						game.players.find((p) => p.color === game.currentTurn)
							?.name || ""
					}
					onRollDice={game.onRollDice}
					isRolling={game.state === LudoClientState.DICE_ROLLING}
					active={true}
				/>
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
					highlights={game.highlights}
					onClicks={game.onClicks}
					color={Color.BLUE}
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
					highlights={game.highlights}
					onClicks={game.onClicks}
					color={Color.YELLOW}
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
