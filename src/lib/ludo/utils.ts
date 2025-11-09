import { MENU_BLUE_PAWN_COLOR, MENU_GREEN_PAWN_COLOR, MENU_RED_PAWN_COLOR, MENU_YELLOW_PAWN_COLOR } from "./constants";

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