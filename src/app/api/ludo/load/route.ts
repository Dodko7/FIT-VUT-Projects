import { NextResponse } from "next/server";
import type { MenuGame, TypedResult } from "~/lib/ludo/types";
import { db as prisma } from "~/server/db";

/**
 * Returns all Ludo games from the database.
 */
export async function GET(): Promise<NextResponse<TypedResult<MenuGame[]>>> {
	try {
		const games = await prisma.game
			.findMany({
				select: {
					id: true,
                    name: true,
					lastPlayed: true,
					players: true,
				},
			})
			.then((dbGames) =>
				dbGames.map((dbGame) => ({
					id: dbGame.id,
                    name: dbGame.name,
					lastPlayed: dbGame.lastPlayed,
					players: dbGame.players.filter((p) => !p.isBot).length,
					bots: dbGame.players.filter((p) => p.isBot).length,
				})),
			);

		return NextResponse.json({
            success: true,
            value: games,
        });
	} catch (error) {
		return NextResponse.json({
			success: false,
			error: error instanceof Error ? error.message : "Unknown error",
		});
	}
}