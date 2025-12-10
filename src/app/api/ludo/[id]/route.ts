import { NextResponse, type NextRequest } from "next/server";
import { db as prisma } from "~/server/db";

/**
 * Route handler to load a Ludo game by ID.
 * @param Request The incoming request.
 * @param params The parameters containing the game ID.
 * @returns A response with the game data or an error message.
 */
export async function GET(
	Request: NextRequest,
	params: Promise<{ id: string }>,
): Promise<NextResponse> {
	const id = parseInt((await params).id, 10);
	if (isNaN(id)) {
		return NextResponse.json(
			{ success: false, error: "Invalid game ID" },
			{ status: 400 },
		);
	}

	return await prisma.game
		.findUnique({
			where: { id: id },
			include: {
				players: {
					include: {
						pawns: true,
					},
				},
			},
		})
		.then((game) => {
			if (!game) {
				return NextResponse.json(
					{ success: false, error: "Game not found" },
					{ status: 404 },
				);
			}
			return NextResponse.json({ success: true, value: game });
		})
		.catch((error) => {
			console.error("Error loading game:", error);
			return NextResponse.json(
				{ success: false, error: "Internal server error" },
				{ status: 500 },
			);
		});
}
