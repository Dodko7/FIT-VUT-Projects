import type { Color } from "@prisma/client";
import type { Result } from "../types";

/**
 * Leaves the game as the player with the given color.
 * @param gameName The name of the game.
 * @param color The color of the player leaving.
 * @returns A promise resolving to the result of the leave operation.
 */
export async function LeaveGame(gameName: string, color: Color): Promise<void> {
	await fetch(`/api/ludo/${gameName}/leave`, {
		method: "POST",
		headers: {
			"Content-Type": "application/json",
		},
		body: JSON.stringify({ color }),
	});
}
