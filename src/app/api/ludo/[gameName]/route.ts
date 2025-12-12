import { NextResponse, type NextRequest } from "next/server";
import { db as prisma } from "~/server/db";

/**
 * Route handler to load a Ludo game by name.
 * @param Request The incoming request.
 * @param params The parameters containing the game name.
 * @returns A response with the game data or an error message.
 */
export async function GET(
	Request: NextRequest,
	params: Promise<{ gameName: string }>,
): Promise<NextResponse> {
	const gameName = (await params).gameName;
	if (!gameName) {
		return NextResponse.json(
			{ success: false, error: "Invalid game name" },
			{ status: 400 },
		);
	}

	return await prisma.game
		.findUnique({
			where: { name: gameName },
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
			return NextResponse.json(
				{ success: false, error: "Internal server error" },
				{ status: 500 },
			);
		});
}
