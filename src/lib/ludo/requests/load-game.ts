import type { Result } from "~/lib/ludo/types";

/**
 * Reacts to the user loading a game from a JSON file.
 * @param e The file input change event.
 * @return A promise resolving to a Result indicating success or failure.
 */
export async function LoadFromJSON(e: React.ChangeEvent<HTMLInputElement>): Promise<Result> {
        return { success: true };
}