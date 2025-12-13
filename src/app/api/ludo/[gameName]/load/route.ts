import { NextResponse } from "next/server";
import { db as prisma } from "~/server/db";

/**
 * Loads a Ludo game by name.
 * @param param1 Game name parameter from the URL.
 * @returns The game data if found.
 */
export async function GET(
    request: Request,
	{ params }: { params: Promise<{ gameName: string }> }
): Promise<NextResponse> {
	try {
		const gameName = (await params).gameName;

		if (!gameName) {
			return NextResponse.json(
				{ success: false, error: "Invalid game name" },
				{ status: 400 },
			);
		}

		const game = await prisma.game.findUnique({
			where: { name: gameName },
			include: {
				players: {
					include: {
						pawns: true,
					},
				},
			},
		});

		if (!game) {
			return NextResponse.json(
				{ success: false, error: "Game not found" },
				{ status: 404 },
			);
		}

		return NextResponse.json({ success: true, value: game });
	} catch (error) {
		console.error("Error loading game:", error);
		return NextResponse.json(
			{ success: false, error: "Internal server error" },
			{ status: 500 },
		);
	}
}
