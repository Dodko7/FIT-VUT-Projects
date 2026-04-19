import { NextResponse, type NextRequest } from "next/server";
import type { FullGame } from "~/lib/ludo/types";
import { db as prisma } from "~/server/db";

/**
 * Creates a new DB game from JSON sent in the request body.
 * @param request The incoming request containing JSON data.
 */
export async function POST(request: NextRequest): Promise<NextResponse> {
	let jsonContent: FullGame;
	jsonContent = await request.json().then((data) => data.jsonContent);

	try {
		// Check if game with this name exists
		const existingGameResponse = await prisma.game.findUnique({
			where: { name: jsonContent.name },
		});

		if (existingGameResponse) {
			return NextResponse.json(
				{
					success: false,
					error: `Game with name ${jsonContent.name} already exists.`,
				},
				{ status: 400 },
			);
		}

		// Create new game
		const newGame = await prisma.game.create({
			data: {
				name: jsonContent.name,
				lastPlayed: new Date(),
				over: jsonContent.over,
				turn: jsonContent.turn,
				extraTurn: jsonContent.extraTurn,
			},
		});

		// Create players and pawns
		for (const player of jsonContent.players) {
			const createdPlayer = await prisma.player.create({
				data: {
					name: player.name,
					isBot: player.isBot,
					color: player.color,
					gameId: newGame.id,
					finished: player.finished,
				},
			});

			for (const pawn of player.pawns) {
				await prisma.pawn.create({
					data: {
						position: pawn.position,
						gameId: newGame.id,
						playerId: createdPlayer.id,
						color: player.color,
						inHome: pawn.inHome,
					},
				});
			}
		}

		return NextResponse.json({ success: true }, { status: 201 });
	} catch (error) {
		return NextResponse.json(
			{
				success: false,
				error: `Failed to load game from JSON: ${error instanceof Error ? error.message : String(error)}`,
			},
			{ status: 500 },
		);
	}
}
