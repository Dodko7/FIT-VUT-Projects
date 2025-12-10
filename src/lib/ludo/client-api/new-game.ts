import type { NewGameRequest, Result, TypedResult } from "../types";

/**
 * New game page submit handler.
 * @param data Inputs from the form.
 * @param numberOfPlayers The selected number of players.
 * @param botsOn Whether bots are enabled.
 * @returns A promise resolving to a Result indicating success or failure.
 */
export async function CreateNewGame(
	data: FormData,
	numberOfPlayers: 1 | 2 | 3 | 4 | null,
	botsOn: boolean,
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

	let playerNames: string[] = [];
	for (let i = 0; i < numberOfPlayers; i++) {
		const playerName = data.get(`ludo-input-player-${i + 1}`);
		if (
			!playerName ||
			typeof playerName !== "string" ||
			playerName.trim() === ""
		) {
			return {
				success: false,
				error: `No name specified for Player ${i + 1}!`,
			};
		} else if (playerNames.includes(playerName.trim())) {
			return {
				success: false,
				error: `${playerName.trim()} has already been used as a name!`,
			};
		}
		playerNames.push(playerName);
	}

	const request: NewGameRequest = {
		name: gameName.toString(),
		playerNames: playerNames,
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
