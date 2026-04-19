/**
 * Enum of client side ludo game states.
 */
export enum LudoClientState {
	// Dice is rolling
	DICE_ROLLING,

	// Awaiting player move (to click dice)
	AWAITING_PLAYER_MOVE,

	// Awaiting player to click a pawn to move
	AWAITING_PAWN_SELECTION,

	// Awaiting player to click a spot to move to
	AWAITING_SPOT_SELECTION,

	// Game over
	GAME_OVER,
}

export default LudoClientState;
