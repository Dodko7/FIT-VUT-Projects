/**
 * API Route: /api/snake/player/[playerName]
 * 
 * Štatistiky konkrétneho hráča
 * GET: získanie štatistík a histórie hier pre daného hráča
 * Params: playerName
 * Query: includeGames (boolean) - zahrnie históriu hier
 * 
 * @author Jozef Ondrejicka
 */

import { NextResponse } from "next/server";
import { getPlayerStats, getPlayerGames } from "~/lib/db/snake";

/**
 * GET - Získanie štatistík hráča
 * Params: playerName
 * Query: ?includeGames=true (voliteľné - zahrnie históriu hier)
 */
export async function GET(
	request: Request,
	{ params }: { params: { playerName: string } },
) {
	try {
		const playerName = params.playerName;
		const { searchParams } = new URL(request.url);
		const includeGames = searchParams.get("includeGames") === "true";

		if (!playerName) {
			return NextResponse.json(
				{ success: false, error: "Missing playerName parameter" },
				{ status: 400 },
			);
		}

		// Dekódovanie URL encoded mena
		const decodedPlayerName = decodeURIComponent(playerName);

		// Získanie štatistík
		const stats = await getPlayerStats(decodedPlayerName);

		if (!stats) {
			return NextResponse.json(
				{ success: false, error: "Player not found" },
				{ status: 404 },
			);
		}

		// Voliteľne pridáme históriu hier
		let recentGames: Awaited<ReturnType<typeof getPlayerGames>> = [];
		if (includeGames) {
			recentGames = await getPlayerGames(decodedPlayerName, 10);
		}

		// Vypočítame priemerné skóre
		const averageScore =
			stats.totalGames > 0 ?
				Math.round(stats.totalScore / stats.totalGames)
			:	0;

		const response = {
			...stats,
			averageScore,
			recentGames: includeGames ? recentGames : undefined,
		};

		return NextResponse.json({
			success: true,
			data: response,
		});
	} catch (error) {
		console.error("Error fetching player stats:", error);
		return NextResponse.json(
			{ success: false, error: "Failed to fetch player stats" },
			{ status: 500 },
		);
	}
}
