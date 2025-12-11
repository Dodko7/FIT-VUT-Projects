/**
 * Enum of client side ludo game states.
 */
export enum LudoClientState {
	// Waiting for another player to roll dice or make a move
	WAITING,

	// Dice is rolling
	DICE_ROLLING,

	// Current player can roll the dice
	TURN_ROLL,

	// Current player can move a pawn
	TURN_MOVE,

	// Game over
	GAME_OVER,
};

export default LudoClientState;
