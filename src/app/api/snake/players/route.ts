/**
 * API Route: /api/snake/players
 * Zoznam všetkých hráčov
 * 
 * @author Igor Lacko
 */

import { NextResponse } from "next/server";
import { getAllPlayers, getTopPlayers } from "~/lib/db/snake";

/**
 * GET - Získanie zoznamu hráčov
 * Query params:
 *   - top?: boolean (ak true, vráti top hráčov podľa best score)
 *   - limit?: number (max počet výsledkov, default 50)
 */
export async function GET(request: Request) {
	try {
		const { searchParams } = new URL(request.url);
		const isTop = searchParams.get("top") === "true";
		const limitParam = searchParams.get("limit");
		const limit = limitParam ? parseInt(limitParam) : 50;

		// Validácia limitu
		if (isNaN(limit) || limit < 1 || limit > 100) {
			return NextResponse.json(
				{ success: false, error: "Limit must be between 1 and 100" },
				{ status: 400 },
			);
		}

		// Získanie hráčov
		const players = isTop ?
			await getTopPlayers(limit)
		:	await getAllPlayers(limit);

		return NextResponse.json({
			success: true,
			data: players,
		});
	} catch (error) {
		console.error("Error fetching players:", error);
		return NextResponse.json(
			{ success: false, error: "Failed to fetch players" },
			{ status: 500 },
		);
	}
}
