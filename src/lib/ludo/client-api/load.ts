import type { FullGame, MenuGame, Result, TypedResult } from "~/lib/ludo/types";

/**
 * Reacts to the user loading a game from a JSON file.
 * @param e The file input change event.
 * @return A promise resolving to a Result indicating success or failure.
 */
export async function LoadFromJSON(
	e: React.ChangeEvent<HTMLInputElement>,
): Promise<Result> {
	const file = e.target.files?.[0];
	if (!file) return { success: false, error: "No file selected." };

	const reader = new FileReader();
	reader.onload = async (e) => {
		try {
			const text = e.target?.result as string;
			const data = JSON.parse(text);

			// Send to backend
			await fetch("/api/ludo/import", {
				method: "POST",
				headers: { "Content-Type": "application/json" },
				body: JSON.stringify(data),
			}).catch(() => null);
		} catch (err) {}
	};
	reader.readAsText(file);
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
