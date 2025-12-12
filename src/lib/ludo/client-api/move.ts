import type { MovePawnResult, TypedResult } from "../types";

/**
 * Makes an API call to move a pawn in a specific game.
 * @param gameName The name of the game.
 * @param pawnId The ID of the pawn to move.
 * @param position The new position for the pawn.
 * @returns A promise resolving to a Result indicating success or failure.
 */
export default async function MovePawn(
	gameName: string,
	pawnId: number,
	position: number,
): Promise<TypedResult<MovePawnResult>> {
	return await fetch(`/api/ludo/${gameName}/move`, {
		method: "POST",
		headers: {
			"Content-Type": "application/json",
		},
		body: JSON.stringify({
			pawnId,
			position,
		}),
	}).then(async (res) => {
		const data: TypedResult<MovePawnResult> = await res.json();
		if (data.success) {
			return data;
		} else {
			return {
				success: false,
				error: data.error,
			};
		}
	}).catch((error) => {
		return {
			success: false,
			error: error.message,
		};
	}) as TypedResult<MovePawnResult>;
}
