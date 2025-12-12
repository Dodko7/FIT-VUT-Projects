import type { Result, RollDiceResult, TypedResult } from "../types";

/**
 * Calls the API to roll the dice for a specific game.
 * @param gameName The name of the game.
 * @param color The color of the player rolling.
 * @returns The result of the roll, available moves, and any errors.
 */
export async function RollDice(
	gameName: string,
): Promise<TypedResult<RollDiceResult>> {
	return await fetch(`/api/ludo/${gameName}/roll`, {
		method: "POST",
		headers: {
			"Content-Type": "application/json",
		},
	}).then(async (res) => {
		const data: TypedResult<RollDiceResult> = await res.json();
		if (data.success) {
			return data;
		} else {
			throw new Error(data.error);
		}
	});
}
