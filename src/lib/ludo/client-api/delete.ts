/**
 * Makes a DELETE request to delete a Ludo game by its name.
 * @param gameName The name of the game to delete.
 */
export async function DeleteGame(gameName: string): Promise<void> {
    await fetch(`/api/ludo/${gameName}/delete`, {
        method: "DELETE"
    }).then((res) => {
        if (!res.ok) {
            throw new Error(`Failed to delete game with name ${gameName}: ${res.statusText}`);
        }
    });
}