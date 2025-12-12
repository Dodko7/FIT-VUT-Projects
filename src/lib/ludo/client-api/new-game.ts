import { Color } from "@prisma/client";
import type { NewGameRequest, TypedResult } from "../types";

/**
 * New game page submit handler.
 * @param data Inputs from the form.
 * @param numberOfPlayers The selected number of players.
 * @param botsOn Whether bots are enabled.
 * @param hostColor The color selected by the host.
 * @returns A promise resolving to a Result indicating success or failure.
 */
export async function CreateNewGame(
	data: FormData,
	numberOfPlayers: 1 | 2 | 3 | 4 | null,
	botsOn: boolean,
	hostColor: Color,
): Promise<TypedResult<number>> {
	// Validate numbers
	if (numberOfPlayers === null) {
		return {
			success: false,
			error: "No players specified!",
		};
	} else if (numberOfPlayers === 1 && !botsOn) {
		return {
			success: false,
			error: "You can't play alone! Add more players or turn bots on.",
		};
	}

	// Validate inputs
	const gameName = data.get("gameName");
	if (!gameName) {
		return {
			success: false,
			error: "No game name specified!",
		};
	}

	// Get player names
	const redPlayerName = data.get("player-name-input-red")?.toString().trim();
	const yellowPlayerName = data
		.get("player-name-input-yellow")
		?.toString()
		.trim();
	const bluePlayerName = data
		.get("player-name-input-blue")
		?.toString()
		.trim();
	const greenPlayerName = data
		.get("player-name-input-green")
		?.toString()
		.trim();

	let players: { name: string; color: Color }[] = [];
	if (redPlayerName) {
		players.push({ name: redPlayerName, color: Color.RED });
	}
	if (yellowPlayerName) {
		players.push({ name: yellowPlayerName, color: Color.YELLOW });
	}
	if (bluePlayerName) {
		players.push({ name: bluePlayerName, color: Color.BLUE });
	}
	if (greenPlayerName) {
		players.push({ name: greenPlayerName, color: Color.GREEN });
	}

	// Build request object
	const request: NewGameRequest = {
		name: gameName.toString(),
		players: players,
		bots: botsOn,
	};

	// POST to API
	return (await fetch("/api/ludo/new-game", {
		method: "POST",
		headers: {
			"Content-Type": "application/json",
		},
		body: JSON.stringify(request),
	})
		.then(async (res) => {
			if (res.ok) {
				// Idk why the casting is even needed here, ts is disgusting
				return {
					success: true,
					value: (
						(await res.json()) as { success: true; value: number }
					).value,
				};
			} else {
				const resJson = await res.json();
				return {
					success: false,
					error:
						resJson.success ?
							"Unknown error occurred."
						:	resJson.error,
				};
			}
		})
		.catch((err) => ({
			success: false,
			error:
				err instanceof Error ? err.message : "Unknown error occurred.",
		}))) as unknown as Promise<TypedResult<number>>;
}
