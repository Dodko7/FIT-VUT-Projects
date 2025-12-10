import { NextResponse } from "next/server";
import { db as prisma } from "~/server/db";

/**
 * Loads a Ludo game by ID.
 * @param param1 Game ID parameter from the URL.
 * @returns The game data if found.
 */
export async function GET(
	{ params }: { params: Promise<{ id: string }> }
): Promise<NextResponse> {
	try {
		const id = parseInt((await params).id, 10);

		if (isNaN(id)) {
			return NextResponse.json(
				{ success: false, error: "Invalid game ID" },
				{ status: 400 },
			);
		}

		const game = await prisma.game.findUnique({
			where: { id: id },
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
