import type { FullGame, MenuGame, Result, TypedResult } from "~/lib/ludo/types";

/**
 * Reacts to the user loading a game from a JSON file.
 * @param e The file input change event.
 * @return A promise resolving to a Result indicating success or failure.
 */
export async function LoadFromJSON(
	e: React.ChangeEvent<HTMLInputElement>,
): Promise<Result> {
	return { success: true };
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
 * Makes an API call to load the current active game.
 * @returns A promise resolving to a TypedResult containing the FullGame object.
 */
export async function LoadCurrentGame(): Promise<FullGame> {
	const response = await fetch("/api/ludo/load/current");
	const result = (await response.json()) as TypedResult<FullGame>;

	if (!result.success) {
		throw new Error(result.error);
	}

	return result.value;
}

/**
 * Loads a Ludo game by its name.
 * @param gameName The name of the game to load.
 * @returns A promise resolving to a Game/Full
 */
export async function LoadGameByName(gameName: string): Promise<FullGame> {
	const response = await fetch(`/api/ludo/${gameName}/load`);
	const result = (await response.json()) as TypedResult<FullGame>;

	if (!result.success) {
		throw new Error(result.error);
	}

	return result.value;
}
