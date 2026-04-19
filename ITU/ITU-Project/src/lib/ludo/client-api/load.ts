import type { FullGame, MenuGame, Result, TypedResult } from "~/lib/ludo/types";

/**
 * Reacts to the user loading a game from a JSON file.
 * @param e The file input change event.
 * @return A promise resolving to a Result indicating success or failure.
 */
export async function LoadFromJSON(
	jsonContent: any,
): Promise<Result> {
	return await fetch(`/api/ludo/load-from-json`, {
		method: "POST",
		headers: {
			"Content-Type": "application/json",
		},
		body: JSON.stringify({ jsonContent }),
	}).then(async (res) => {
		const data: Result = await res.json();
		if (data.success) {
			return {
				success: true,
			};
		} else {
			return {
				success: false,
				error: data.error,
			};
		}
	});
}

/**
 * Makes a API call to load all saved Ludo games.
 * @returns A promise resolving to a TypedResult containing an array of MenuGame objects.
 */
export async function LoadAllGames(): Promise<MenuGame[]> {
	return await fetch("/api/ludo/load", {
		method: "GET",
		headers: {
			"Content-Type": "application/json",
		},
	}).then(async (res) => {
		const data: TypedResult<MenuGame[]> = await res.json();
		if (data.success) {
			return data.value;
		} else {
			throw new Error(data.error);
		}
	});
}

/**
 * Loads a Ludo game by its name.
 * @param gameName The name of the game to load.
 * @returns A promise resolving to a Game/Full
 */
export async function LoadGameByName(
	gameName: string,
): Promise<TypedResult<FullGame>> {
	return (await fetch(`/api/ludo/${gameName}/load`, {
		method: "GET",
		headers: {
			"Content-Type": "application/json",
		},
	})
		.then(async (res) => {
			if (!res.ok) {
				throw new Error(
					`Failed to load game with name ${gameName}: ${res.statusText}`,
				);
			}
			const data: TypedResult<FullGame> = await res.json();
			if (data.success) {
				return data;
			} else {
				return {
					success: false,
					error: data.error,
				};
			}
		})
		.catch((err) => ({
			success: false,
			error:
				err instanceof Error ? err.message : "Unknown error occurred.",
		}))) as unknown as Promise<TypedResult<FullGame>>;
}
