/**
 * Enum of possible pawn spot highlight types.
 */
export enum PawnSpotHighlight {
	// Avaliable pawn (so clicking will select avail. moves)
	AVALIABLE_PAWN,

	// Avaliable move
	AVALIABLE_MOVE,

	// Another pawn of "enemy" that can be destroyed
	OPPONENT_PAWN,

	// Selected pawn
	SELECTED_PAWN,
};

export default PawnSpotHighlight;
