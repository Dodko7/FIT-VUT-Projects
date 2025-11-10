import type { MenuGame, Result, TypedResult } from "~/lib/ludo/types";

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
