import { Color, type Player } from "@prisma/client";
import { NextResponse, type NextRequest } from "next/server";
import type { NewGameRequest } from "~/lib/ludo/types";
import { db as prisma } from "~/server/db";

/**
 * Maps player index to color.
 * @param index The player index (0-3).
 * @returns A Color corresponding to the player index.
 */
function ColorFromIndex(index: number): Color {
	switch (index) {
		case 0:
			return "RED";
		case 1:
			return "YELLOW";
		case 2:
			return "GREEN";
		case 3:
			return "BLUE";
		default:
			throw new Error("Invalid player index for color assignment");
	}
}

/**
 * Creates a new player.
 * @param name The player's name.
 * @param isBot Whether the player is a bot.
 * @param color The player's color.
 * @param gameId The ID of the game the player belongs to.
 */
async function CreatePlayer(
	name: string,
	isBot: boolean,
	color: Color,
	gameId: number,
): Promise<Player> {
	return await prisma.player.create({
		data: {
			name: name,
			isBot: isBot,
			color: color,
			gameId: gameId,
		},
	});
}

/**
 * Initializes Pawn models for a player.
 * @param playerId The ID of the player.
 * @param color The color (for starting position).
 */
async function CreatePawnsForPlayer(
	playerId: number,
	color: Color,
): Promise<void> {
	const startingPositions: Record<Color, number[]> = {
		RED: [-1, -2, -3, -4],
		YELLOW: [-5, -6, -7, -8],
		GREEN: [-9, -10, -11, -12],
		BLUE: [-13, -14, -15, -16],
	};

	const positions = startingPositions[color];
	for (const pos of positions) {
		await prisma.pawn.create({
			data: {
				position: pos,
				playerId: playerId,
				color: color,
			},
		});
	}
}

/**
 * Creates a new game and sets it as active.
 */
export async function POST(request: NextRequest): Promise<NextResponse> {
	try {
		const reqData: NewGameRequest = await request.json();
		const { name, bots, players } = reqData;

		// Check if game with the same name exists
		if (
			await prisma.game.findFirst({
				where: { name: name },
			})
		) {
			return NextResponse.json(
				{
					success: false,
					error: "A game with this name already exists!",
				},
				{ status: 409 },
			);
		}

		// Create game
		const newGame = await prisma.game.create({
			data: {
				name: name,
				lastPlayed: new Date(),
			},
		});

		// Create players
		for (const player of players) {
			const createdPlayer = await CreatePlayer(
				player.name,
				false,
				player.color,
				newGame.id,
			);
			await CreatePawnsForPlayer(createdPlayer.id, player.color);
		}

		// Create bots if enabled
		const botcount = 4 - players.length;
		const colors = [0, 1, 2, 3]
			.map(ColorFromIndex)
			.filter((c) => !players.some((p) => p.color === c));
		if (bots) {
			for (let i = 0; i < botcount; i++) {
				const botName = `Bot ${i + 1}`;
				const botColor = colors[i];
				const createdBot = await CreatePlayer(
					botName,
					true,
					botColor!,
					newGame.id,
				);
				await CreatePawnsForPlayer(createdBot.id, botColor!);
			}
		}

		// Return OK
		return NextResponse.json({
			success: true,
			value: newGame.name,
		});
	} catch (error) {
		return NextResponse.json(
			{
				success: false,
				error: error instanceof Error ? error.message : "Unknown error",
			},
			{ status: 500 },
		);
	}
}
