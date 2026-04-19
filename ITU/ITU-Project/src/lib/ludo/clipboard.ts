import type { FullGame } from "./types";

/**
 * Copies the given game state to the clipboard as a formatted JSON string.
 * @param game The full game state to copy.
 */
export default async function CopyGameToClipboard(
	game: FullGame,
): Promise<void> {
    // Ehhh this should run always on the client, but just in case
	if (typeof navigator === "undefined" || !navigator.clipboard) {
		throw new Error("Clipboard API is not available.");
	}
	const gameJSON = JSON.stringify(game, null, 2);
	await navigator.clipboard.writeText(gameJSON);
}
