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
	COLOR_CONFIGS,
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
	HighlightedPawnSpot,
	PawnGameState,
	PawnPosition,
	PawnSpotOnClicks,
	PlayerGameState,
} from "./types";
import { Color, type Pawn } from "@prisma/client";
import PawnSpotHighlight from "./enum/pawn-spot-highlight";

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
	highlihts: HighlightedPawnSpot[],
	onClicks: PawnSpotOnClicks[],
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
			const highlight = highlihts.find(
				(h) => h.position === searchedPos,
			);
			const onClickObj = onClicks.find(
				(o) => o.position === searchedPos,
			);

			let highlightType: PawnSpotHighlight | undefined = undefined;
			if (highlight) {
				highlightType = highlight.highlight;
			}

			const onClick = onClickObj ? onClickObj.onClick : undefined;

			const className =
				searchedPos >= TOTAL_BOARD_POSITIONS ?
					config.homeClass
				:	"ludo-empty-circle";

			const arrow = i === config.arrowIndex ? config.arrow : undefined;

			return {
				className,
				size: PAWN_SPOT_BOARD_SIZE,
				hasPawn: !!position,
				pawnProps:
					position ?
						{
							color: ColorEnumToPawnColor(position.color),
							width: PAWN_SPOT_BOARD_SIZE - 10,
							height: PAWN_SPOT_BOARD_SIZE - 10,
						}
					:	undefined,
				arrow,
				highlight: highlightType,
				onClick,
			};
		},
	);

	return { pawnSpotProps: props };
}

/**
 * Computes available moves for a selected pawn.
 * @param selected The selected pawn.
 * @param allPawns All pawns in the game.
 * @param color The color of the current player.
 * @param diceRoll The result of the dice roll.
 */
export function GetAvailableMoves(
	selected: Pawn,
	allPawns: Pawn[],
	color: Color,
	diceRoll: number,
): HighlightedPawnSpot[] {
	const { position } = selected;
    const config = COLOR_CONFIGS[color];
    const availableMoves: HighlightedPawnSpot[] = [];

    // Helper function to determine if a target position is blocked or capturable
    const getTargetHighlight = (targetPos: number): PawnSpotHighlight | null => {
        const blockingPawn = allPawns.find(p => p.position === targetPos);
        if (!blockingPawn) {
            return PawnSpotHighlight.AVALIABLE_MOVE; // Position is free
        }

        if (blockingPawn.color === color) {
            return null; // Blocked by same-color pawn (on main board or home path)
        }
        
        // Opponent pawn is present (capture)
        return PawnSpotHighlight.OPPONENT_PAWN;
    };

    // --- 1. Pawn is in the Starting Area (position < 0) ---
    if (position < 0) {
        if (diceRoll === 6) {
            const finalPos = config.BOARD_ENTRY;
            const status = getTargetHighlight(finalPos);
            
            // Allow if it's a capture or a free spot (status !== null means it's not blocked by self)
            if (status !== null) { 
                 availableMoves.push({ position: finalPos, highlight: status });
            }
        }
        return availableMoves;
    }

    // --- 2. Pawn is on the Main Board (0 <= position <= 43) or Home Path (position >= 44) ---
    let currentPos = position;

    for (let i = 1; i <= diceRoll; i++) {
        let nextPos: number;
        
        // A. Calculate the next potential position
        if (currentPos >= config.HOME_START) {
            nextPos = currentPos + 1; // Already in Home Path, move linearly
        } else {
            // On Main Board
            if (currentPos === config.HOME_ENTRY) {
                nextPos = config.HOME_START; // Reached entry point, move to Home Path Start
            } else {
                nextPos = (currentPos + 1) % TOTAL_BOARD_POSITIONS; // Move on Main Board (circular)
            }
        }

        // B. Check for overshooting Home End
        if (nextPos > config.HOME_END) {
            return availableMoves; 
        }

        // C. Check for blockage ONLY on the final step (i == diceRoll)
        if (i === diceRoll) {
            const status = getTargetHighlight(nextPos);

            // If status is not null, it means it's either AVAILABLE_MOVE or OPPONENT_PAWN
            if (status !== null) {
                availableMoves.push({ position: nextPos, highlight: status });
            }
            
            return availableMoves;
        }

        // D. Update position for the next iteration (only if not the final step)
        currentPos = nextPos;
    }

    return availableMoves;
}

export function GetMoveSelectionHighlights() {}
export function GetMoveSelectionOnClicks() {}