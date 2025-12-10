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
import type { BoardPartProps, FullPlayer, PawnPosition } from "./types";
import { Color } from "@prisma/client";

/**
 * Maps a color to the player number and returns a string "Player X".
 */
export function PlayerNameFromColor(color: string): string {
	switch (color) {
		case MENU_RED_PAWN_COLOR:
			return "Player 1";
		case MENU_YELLOW_PAWN_COLOR:
			return "Player 2";
		case MENU_BLUE_PAWN_COLOR:
			return "Player 3";
		case MENU_GREEN_PAWN_COLOR:
			return "Player 4";
		default:
			return "Unknown Player";
	}
}

/**
 * Returns the number of completed pawns for a given player.
 */
export function CompletedPawnsFromColor(player: FullPlayer): number {
	return player.pawns.filter((pawn) => pawn.inHome).length;
}

/**
 * Returns a boolean array of pawns indicating if they are in the start area.
 */
export function PawnsInStartFromPlayer(player: FullPlayer): boolean[] {
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
	positions: PawnPosition[],
): BoardPartProps {
	console.log("Getting pawn spot props for part:", part);
	let start = 0;
	let homeStart = 0;
	let homeClass = "";
	if (part === BOARD_TOP_PART) {
		start = BOARD_TOP_START;
		homeStart = PAWN_GREEN_HOME_1;
		homeClass = "ludo-green-circle";
	} else if (part === BOARD_RIGHT_PART) {
		start = BOARD_RIGHT_START;
		homeStart = PAWN_YELLOW_HOME_1;
		homeClass = "ludo-yellow-circle";
	} else if (part === BOARD_BOTTOM_PART) {
		start = BOARD_BOTTOM_START;
		homeStart = PAWN_BLUE_HOME_1;
		homeClass = "ludo-blue-circle";
	} else {
		start = BOARD_LEFT_START;
		homeStart = PAWN_RED_HOME_1;
		homeClass = "ludo-red-circle";
	}

	const isVerticalPart =
		part === BOARD_TOP_PART || part === BOARD_BOTTOM_PART;
	const props: PawnSpotProps[] = Array.from(
		{ length: 15 },
		(_, i): PawnSpotProps => {
			console.log(
				`Computing pawn spot prop for index ${i} in part ${part}`,
			);
			const searchedPos = GetSearchedPosition(i, part);

			console.log("Checking position:", searchedPos);
			const position = positions.find(
				(pos) => pos.position === searchedPos,
			);
			const className =
				searchedPos >= TOTAL_BOARD_POSITIONS ? homeClass : (
					"ludo-empty-circle"
				);
			
            let arrow: "up" | "down" | "left" | "right" | undefined = undefined;
            if (part === BOARD_TOP_PART && i === 1) arrow = "down";
            else if (part === BOARD_RIGHT_PART && i === 9) arrow = "left";
            else if (part === BOARD_BOTTOM_PART && i === 13) arrow = "up";
            else if (part === BOARD_LEFT_PART && i === 5) arrow = "right";

			if (position) {
				const color = ColorEnumToPawnColor(position.color);
				return {
					className,
					size: PAWN_SPOT_BOARD_SIZE,
					hasPawn: true,
					pawnProps: {
						color,
						width: PAWN_SPOT_BOARD_SIZE - 10,
						height: PAWN_SPOT_BOARD_SIZE - 10,
					},
                    arrow,
				};
			} else {
				return {
					className,
					size: PAWN_SPOT_BOARD_SIZE,
					hasPawn: false,
                    arrow,
				};
			}
		},
	);

	return { pawnSpotProps: props };
}
