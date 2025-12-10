import type { Color, Prisma } from "@prisma/client";
import type { PawnSpotProps } from "~/components/games/ludo/board/pawn-spot";

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
	playerNames: string[];
	bots: boolean;
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
type DiceRoll = 1 | 2 | 3 | 4 | 5 | 6;

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
