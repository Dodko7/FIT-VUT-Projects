import type { Result } from "../types";

/**
 * Calls the API to roll the dice for a specific game.
 * @param gameId The ID of the game.
 * @param color The color of the player rolling.
 * @returns The result of the roll (success/failure).
 */
export async function RollDice(gameId: number, color: string): Promise<Result> {
    return await fetch(`/api/ludo/${gameId}/roll`, {
        method: "POST",
        headers: {
            "Content-Type": "application/json",
        },
        body: JSON.stringify({ color }),
    }).then(async (res) => {
        const data: Result = await res.json();
        return data;
    }).catch((err) => ({
        success: false,
        error: err instanceof Error ? err.message : "Unknown error occurred.",
    }));
}
