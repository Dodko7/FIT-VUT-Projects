import type { Color, Prisma } from "@prisma/client";
import type { PawnSpotProps } from "~/components/games/ludo/board/pawn-spot";
import type LudoClientState from "./client-state";

/**
 * General purpose type for a promise which may or may not succeed.
 */
export type Result =
	| {
			success: true;
	  }
	| {
			success: false;
			error: string;
	  };

/**
 * The same but for a promise returning a value.
 */
export type TypedResult<T> =
	| {
			success: true;
			value: T;
	  }
	| {
			success: false;
			error: string;
	  };

/**
 * Subset of the Game model for the menu where saved games are shown.
 */
export type MenuGame = {
	id: number;
	name: string;
	lastPlayed: Date;
	players: number;
	bots: number;
};

/**
 * Type for new game requests.
 */
export type NewGameRequest = {
	name: string;
	nofPlayers: number;
	hostName: string;
	bots: boolean;
	hostColor: Color;
};

/**
 * Full game model.
 */
export type FullGame = Prisma.GameGetPayload<{
	include: {
		players: {
			include: {
				pawns: true;
			};
		};
	};
}>;

/**
 * Full player model.
 */
export type FullPlayer = Prisma.PlayerGetPayload<{
	include: {
		pawns: true;
	};
}>;

/**
 * Board part props. This is here, because it is shared between two components.
 */
export type BoardPartProps = {
	pawnSpotProps: PawnSpotProps[];
	extraClassNames?: string;
};

/**
 * For computing pawn spot props for board parts.
 */
export type PawnPosition = {
	position: number;
	color: Color;
};

/**
 * Valid dice roll values.
 */
export type DiceRoll = 1 | 2 | 3 | 4 | 5 | 6;

/**
 * For rolling the dice component.
 */
export type DiceProps = {
	lastRoll: DiceRoll | null;
	isRolling: boolean;
};

/**
 * This is precisely what it is called.
 */
type LinkWithText = {
	text: string;
	link: string;
};

/**
 * Props for the error page. At least one redirect link or a close handler should be provided.
 */
export type ErrorPageProps = {
	message: string;
	links?: LinkWithText[];
	onClose?: () => void;
};

/**
 * Props for the color picker component.
 */
export type ColorPickerProps = {
	currentColor: Color;
	onColorChange: (newColor: Color) => void;
};

/**
 * Props for the paused page.
 */
export type PausedPageProps = {
	onResume: () => void;
	onQuit: () => Promise<void>;
	onExport: () => Promise<Result>;
};

/**
 * For player game state.
 */
export type PawnGameState = {
	id: number;
	position: number;
	inHome: boolean;
	color: Color;
};

/**
 * Andddd for LudoGameState.
 */
export type PlayerGameState = {
	color: Color;
	name: string;
	pawns: PawnGameState[];
};

/**
 * Represents the state of a Ludo game for a concrete player.
 */
export type LudoGameState = {
	// Connectivity
	isLoading: boolean;
	error: Error | null;

	// Game state
	state: LudoClientState;
	isPaused: boolean;
	diceNumber: DiceRoll | null;
	currentTurn: Color;

	// Entities
	players: PlayerGameState[];
	name: string;

	// Optional, depending on the state (else -1/empty)
	selectedPawnId: number;
	avaliablePawns: number[];
	avaliableMoves: number[];

	// Actions
	onRollDice: () => Promise<void>;
	onSelectPawn: (pawnId: number) => void;
	onMovePawn: (pawnId: number, moveBy: number) => Promise<void>;
	onPauseGame: () => void;
	onResumeGame: () => void;
	onQuitGame: () => void;
};
