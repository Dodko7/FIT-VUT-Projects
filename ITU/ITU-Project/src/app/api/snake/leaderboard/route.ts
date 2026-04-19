/**
 * API Route: /api/snake/leaderboard
 * 
 * Leaderboard pre Snake hru
 * GET: získanie top výsledkov s filtrom podľa typu hry
 * Query params: gameType (CLASSIC/BOX/CAMPAIGN), limit (počet výsledkov)
 * 
 * @author Jozef Ondrejicka
 */

import { NextResponse } from "next/server";
import { getLeaderboard } from "~/lib/db/snake";
import type { SnakeGameType } from "~/lib/types/snake";

/**
 * GET - Získanie leaderboardu
 * Query params:
 *   - gameType?: CLASSIC | BOX | CAMPAIGN (filter podľa typu hry)
 *   - limit?: number (max počet výsledkov, default 10)
 */
export async function GET(request: Request) {
	try {
		const { searchParams } = new URL(request.url);
		const gameType = searchParams.get("gameType") as SnakeGameType | null;
		const limitParam = searchParams.get("limit");
		const limit = limitParam ? parseInt(limitParam) : 10;

		// Validácia game type
		if (
			gameType &&
			!["CLASSIC", "BOX", "CAMPAIGN"].includes(gameType)
		) {
			return NextResponse.json(
				{
					success: false,
					error: "Invalid gameType. Must be CLASSIC, BOX, or CAMPAIGN",
				},
				{ status: 400 },
			);
		}

		// Validácia limitu
		if (isNaN(limit) || limit < 1 || limit > 100) {
			return NextResponse.json(
				{ success: false, error: "Limit must be between 1 and 100" },
				{ status: 400 },
			);
		}

	// Získanie leaderboardu
	const games = await getLeaderboard(gameType ?? undefined, limit);

	// Formátovanie odpovede s pozíciou
	const leaderboard = games.map((game: any, index: number) => ({
		position: index + 1,
		gameId: game.id,
		playerName: game.playerName,
		score: game.score,
		level: game.level,
		gameType: game.gameType,
		date: game.finishedAt ?? game.createdAt,
	}));		return NextResponse.json({
			success: true,
			data: leaderboard,
		});
	} catch (error) {
		console.error("Error fetching leaderboard:", error);
		return NextResponse.json(
			{ success: false, error: "Failed to fetch leaderboard" },
			{ status: 500 },
		);
	}
}
