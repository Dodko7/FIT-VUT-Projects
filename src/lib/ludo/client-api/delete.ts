/**
 * Makes a DELETE request to delete a Ludo game by its ID.
 * @param gameId The ID of the game to delete.
 */
export async function DeleteGame(gameId: string): Promise<void> {
    await fetch(`/api/ludo/${gameId}/delete`, {
        method: "DELETE"
    }).then((res) => { // todo
        if (!res.ok) {
            throw new Error(`Failed to delete game with ID ${gameId}: ${res.statusText}`);
        }
    });
}