import type { PawnSpotProps } from "~/components/games/ludo/board/pawn-spot";
import {
	BOARD_BOTTOM_END,
	BOARD_BOTTOM_PART,
	BOARD_BOTTOM_START,
	BOARD_LEFT_END,
	BOARD_LEFT_PART,
	BOARD_LEFT_START,
	BOARD_RIGHT_END,
	BOARD_RIGHT_PART,
	BOARD_RIGHT_START,
	BOARD_TOP_END,
	BOARD_TOP_PART,
	BOARD_TOP_START,
	BOTTOM_HOME_INDICES,
	INGAME_BLUE_PAWN_COLOR,
	INGAME_GREEN_PAWN_COLOR,
	INGAME_RED_PAWN_COLOR,
	INGAME_YELLOW_PAWN_COLOR,
	LEFT_HOME_INDICES,
	MENU_BLUE_PAWN_COLOR,
	MENU_GREEN_PAWN_COLOR,
	MENU_RED_PAWN_COLOR,
	MENU_YELLOW_PAWN_COLOR,
	PAWN_BLUE_HOME_1,
	PAWN_GREEN_HOME_1,
	PAWN_RED_HOME_1,
	PAWN_SPOT_BOARD_SIZE,
	PAWN_YELLOW_HOME_1,
	RIGHT_HOME_INDICES,
	TOP_HOME_INDICES,
	TOTAL_BOARD_POSITIONS,
} from "./constants";
import type {
	BoardPartProps,
	PawnGameState,
	PawnPosition,
	PlayerGameState,
} from "./types";
import { Color } from "@prisma/client";

/**
 * Returns the number of completed pawns for a given player.
 */
export function CompletedPawnsFromColor(player: PlayerGameState): number {
	return player.pawns.filter((pawn) => pawn.inHome).length;
}

/**
 * Returns a boolean array of pawns indicating if they are in the start area.
 */
export function PawnsInStartFromPlayer(player: PlayerGameState): boolean[] {
	return player.pawns.map((pawn) => pawn.position < 0);
}

/**
 * Returns the pawn color string from the Color enum.
 * @param color The color enum.
 */
function ColorEnumToPawnColor(color: Color): string {
	switch (color) {
		case Color.RED:
			return INGAME_RED_PAWN_COLOR;
		case Color.YELLOW:
			return INGAME_YELLOW_PAWN_COLOR;
		case Color.BLUE:
			return INGAME_BLUE_PAWN_COLOR;
		case Color.GREEN:
			return INGAME_GREEN_PAWN_COLOR;
	}
}

function GetTopBoardPartSearchedPosition(index: number): number {
	if (TOP_HOME_INDICES.includes(index)) {
		return PAWN_GREEN_HOME_1 + TOP_HOME_INDICES.indexOf(index);
	} else {
		return BOARD_TOP_START + index;
	}
}

function GetBottomBoardPartSearchedPosition(index: number): number {
	if (BOTTOM_HOME_INDICES.includes(index)) {
		return PAWN_BLUE_HOME_1 + BOTTOM_HOME_INDICES.indexOf(index);
	} else {
		return BOARD_BOTTOM_START + index;
	}
}

function GetRightBoardPartSearchedPosition(index: number): number {
	if (RIGHT_HOME_INDICES.includes(index)) {
		return PAWN_YELLOW_HOME_1 + RIGHT_HOME_INDICES.indexOf(index);
	} else {
		return BOARD_RIGHT_START + index;
	}
}

function GetLeftBoardPartSearchedPosition(index: number): number {
	if (LEFT_HOME_INDICES.includes(index)) {
		return PAWN_RED_HOME_1 + LEFT_HOME_INDICES.indexOf(index);
	} else {
		// Reverse order from the grid iteration
		if (index <= 3) {
			return BOARD_LEFT_END - (3 - index);
		} else if (index === 4) {
			return BOARD_LEFT_END - 5;
		} else {
			return BOARD_LEFT_START + (index - 6);
		}
	}
}

function GetSearchedPosition(index: number, part: number): number {
	switch (part) {
		case BOARD_TOP_PART:
			return GetTopBoardPartSearchedPosition(index);
		case BOARD_RIGHT_PART:
			return GetRightBoardPartSearchedPosition(index);
		case BOARD_BOTTOM_PART:
			return GetBottomBoardPartSearchedPosition(index);
		case BOARD_LEFT_PART:
			return GetLeftBoardPartSearchedPosition(index);
		default:
			throw new Error("Invalid board part");
	}
}

/**
 * Returns the pawn spot props for a given board part and positions.
 * @param part The board part.
 * @param positions Positions of all pawns.
 */
export function GetPawnSpotPropsForBoardPart(
	part: number,
	positions: PawnGameState[],
): BoardPartProps {
	const config = {
		[BOARD_TOP_PART]: {
			homeClass: "ludo-green-circle",
			arrowIndex: 1,
			arrow: "down" as const,
		},
		[BOARD_RIGHT_PART]: {
			homeClass: "ludo-yellow-circle",
			arrowIndex: 9,
			arrow: "left" as const,
		},
		[BOARD_BOTTOM_PART]: {
			homeClass: "ludo-blue-circle",
			arrowIndex: 13,
			arrow: "up" as const,
		},
		[BOARD_LEFT_PART]: {
			homeClass: "ludo-red-circle",
			arrowIndex: 5,
			arrow: "right" as const,
		},
	}[part];

	if (!config) throw new Error("Invalid board part");

	const props: PawnSpotProps[] = Array.from(
		{ length: 15 },
		(_, i): PawnSpotProps => {
			const searchedPos = GetSearchedPosition(i, part);
			const position = positions.find(
				(pos) => pos.position === searchedPos,
			);
			
			const className =
				searchedPos >= TOTAL_BOARD_POSITIONS
					? config.homeClass
					: "ludo-empty-circle";

			const arrow = i === config.arrowIndex ? config.arrow : undefined;

			return {
				className,
				size: PAWN_SPOT_BOARD_SIZE,
				hasPawn: !!position,
				pawnProps: position
					? {
							color: ColorEnumToPawnColor(position.color),
							width: PAWN_SPOT_BOARD_SIZE - 10,
							height: PAWN_SPOT_BOARD_SIZE - 10,
					  }
					: undefined,
				arrow,
			};
		},
	);

	return { pawnSpotProps: props };
}
