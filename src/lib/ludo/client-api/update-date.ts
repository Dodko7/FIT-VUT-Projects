/**
 * This does exactly what it says it does.
 * @param gameName The name of the game to update.
 */
export default async function UpdateDate(gameName: string): Promise<void> {
	await fetch(`/api/ludo/${gameName}/update-date`, {
		method: "POST",
	})
		.then((_res) => {})
		.catch((_e) => {});
}
