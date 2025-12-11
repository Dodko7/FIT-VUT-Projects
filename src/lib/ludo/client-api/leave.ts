import type { Color } from "@prisma/client";
import type { Result } from "../types";

/**
 * Leaves the game as the player with the given color.
 * @param id The game ID.
 * @param color The color of the player leaving.
 * @returns A promise resolving to the result of the leave operation.
 */
export async function LeaveGame(id: number, color: Color): Promise<Result> {
	return await fetch(`/api/ludo/${id}/leave`, {
		method: "POST",
		headers: {
			"Content-Type": "application/json",
		},
		body: JSON.stringify({ color }),
	})
		.then(async (res) => {
			const data: Result = await res.json();
			return data;
		})
		.catch((err) => ({
			success: false,
			error:
				err instanceof Error ? err.message : "Unknown error occurred.",
		}));
}
