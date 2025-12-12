import { Color } from "@prisma/client";

// Menu pawn colors
export const MENU_RED_PAWN_COLOR = "#F87171";
export const MENU_YELLOW_PAWN_COLOR = "#FACC15";
export const MENU_BLUE_PAWN_COLOR = "#60A5FA";
export const MENU_GREEN_PAWN_COLOR = "#4ADE80";

// In game pawn colors
export const INGAME_RED_PAWN_COLOR = "#c62828";
export const INGAME_YELLOW_PAWN_COLOR = "#f9a825";
export const INGAME_BLUE_PAWN_COLOR = "#1565c0";
export const INGAME_GREEN_PAWN_COLOR = "#2e7d32";

// Pawn starting positions
export const PAWN_1_RED_START = -1;
export const PAWN_2_RED_START = -2;
export const PAWN_3_RED_START = -3;
export const PAWN_4_RED_START = -4;

export const PAWN_1_YELLOW_START = -5;
export const PAWN_2_YELLOW_START = -6;
export const PAWN_3_YELLOW_START = -7;
export const PAWN_4_YELLOW_START = -8;

export const PAWN_1_GREEN_START = -9;
export const PAWN_2_GREEN_START = -10;
export const PAWN_3_GREEN_START = -11;
export const PAWN_4_GREEN_START = -12;

export const PAWN_1_BLUE_START = -13;
export const PAWN_2_BLUE_START = -14;
export const PAWN_3_BLUE_START = -15;
export const PAWN_4_BLUE_START = -16;

// And as arrays!
export const RED_PAWN_START_POSITIONS = [
    PAWN_1_RED_START,
    PAWN_2_RED_START,
    PAWN_3_RED_START,
    PAWN_4_RED_START,
];

export const YELLOW_PAWN_START_POSITIONS = [
    PAWN_1_YELLOW_START,
    PAWN_2_YELLOW_START,
    PAWN_3_YELLOW_START,
    PAWN_4_YELLOW_START,
];

export const GREEN_PAWN_START_POSITIONS = [
    PAWN_1_GREEN_START,
    PAWN_2_GREEN_START,
    PAWN_3_GREEN_START,
    PAWN_4_GREEN_START,
];

export const BLUE_PAWN_START_POSITIONS = [
    PAWN_1_BLUE_START,
    PAWN_2_BLUE_START,
    PAWN_3_BLUE_START,
    PAWN_4_BLUE_START,
];

// Pawn home positions (since there are 44 main board positions, homes start at 44)
export const PAWN_GREEN_HOME_1 = 44;
export const PAWN_GREEN_HOME_2 = 45;
export const PAWN_GREEN_HOME_3 = 46;
export const PAWN_GREEN_HOME_4 = 47;

export const PAWN_YELLOW_HOME_1 = 48;
export const PAWN_YELLOW_HOME_2 = 49;
export const PAWN_YELLOW_HOME_3 = 50;
export const PAWN_YELLOW_HOME_4 = 51;

export const PAWN_BLUE_HOME_1 = 52;
export const PAWN_BLUE_HOME_2 = 53;
export const PAWN_BLUE_HOME_3 = 54;
export const PAWN_BLUE_HOME_4 = 55;

export const PAWN_RED_HOME_1 = 56;
export const PAWN_RED_HOME_2 = 57;
export const PAWN_RED_HOME_3 = 58;
export const PAWN_RED_HOME_4 = 59;

// Pawn props for home positions
const PAWN_HOME_PROPS = {
	width: 50,
	height: 50,
};

export const RED_PAWN_HOME_PROPS = {
	...PAWN_HOME_PROPS,
	color: INGAME_RED_PAWN_COLOR,
};

export const YELLOW_PAWN_HOME_PROPS = {
	...PAWN_HOME_PROPS,
	color: INGAME_YELLOW_PAWN_COLOR,
};

export const BLUE_PAWN_HOME_PROPS = {
	...PAWN_HOME_PROPS,
	color: INGAME_BLUE_PAWN_COLOR,
};

export const GREEN_PAWN_HOME_PROPS = {
	...PAWN_HOME_PROPS,
	color: INGAME_GREEN_PAWN_COLOR,
};

// Board positions
export const BOARD_TOP_PART = 0;
export const BOARD_RIGHT_PART = 1;
export const BOARD_BOTTOM_PART = 2;
export const BOARD_LEFT_PART = 3;

// Board position start/end (goes clockwise)
export const BOARD_TOP_START = 0;
export const BOARD_TOP_END = 10;
export const BOARD_RIGHT_START = 11;
export const BOARD_RIGHT_END = 21;
export const BOARD_BOTTOM_START = 22;
export const BOARD_BOTTOM_END = 32;
export const BOARD_LEFT_START = 33;
export const BOARD_LEFT_END = 43;

// Home indices
export const TOP_HOME_INDICES = [4, 7, 10, 13];
export const RIGHT_HOME_INDICES = [5, 6, 7, 8];
export const BOTTOM_HOME_INDICES = [1, 4, 7, 10];
export const LEFT_HOME_INDICES = [6, 7, 8, 9];

// For circular array
export const NEXT_COLOR = {
	[Color.RED]: Color.YELLOW,
	[Color.YELLOW]: Color.GREEN,
	[Color.GREEN]: Color.BLUE,
	[Color.BLUE]: Color.RED,
};

// Board indices

// Other
export const PAWN_SPOT_BOARD_SIZE = 30;
export const TOTAL_BOARD_POSITIONS = 44;

export const COLOR_CONFIGS = {
    [Color.RED]: {
        HOME_ENTRY: 38,             // Index on the main board where RED enters its home path
        BOARD_ENTRY: 0,             // Index on the main board where RED moves from start
        HOME_START: 56,             // Start index of the home path (PAWN_RED_HOME_1)
        HOME_END: 59,               // End index of the home path (PAWN_RED_HOME_4)
        START_POS_MIN: -4,          // PAWN_4_RED_START
        START_POS_MAX: -1,          // PAWN_1_RED_START
    },
    [Color.YELLOW]: {
        HOME_ENTRY: 7,
        BOARD_ENTRY: 11,
        HOME_START: 48,
        HOME_END: 51,
        START_POS_MIN: -8,
        START_POS_MAX: -5,
    },
    [Color.BLUE]: {
        HOME_ENTRY: 27,
        BOARD_ENTRY: 33,
        HOME_START: 52,
        HOME_END: 55,
        START_POS_MIN: -16,
        START_POS_MAX: -13,
    },
    [Color.GREEN]: {
        HOME_ENTRY: 16,
        BOARD_ENTRY: 22,
        HOME_START: 44,
        HOME_END: 47,
        START_POS_MIN: -12,
        START_POS_MAX: -9,
    },
};