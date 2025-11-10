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