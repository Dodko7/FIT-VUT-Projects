import type { Color } from "@prisma/client";
import { NextResponse, type NextRequest } from "next/server";
import { db as prisma } from "~/server/db";
import { GetSocketServer } from "~/lib/ludo/socket";

/**
 * Called when a player leaves a game.
 * @param Request The incoming request.
 * @param params Contains the game name.
 * @returns A response indicating the result of the leave operation.
 */
export async function POST(
	Request: NextRequest,
	{ params }: { params: Promise<{ gameName: string }> },
): Promise<NextResponse> {
	// Parse id
	const gameName = (await params).gameName;
	if (!gameName) {
		return NextResponse.json(
			{ success: false, error: "Invalid game name" },
			{ status: 400 },
		);
	}

	// Parse color from body
	const body = await Request.json();
	const color: Color = body.color;

	// Find game by name to get id
	const game = await prisma.game.findUnique({
		where: { name: gameName },
	});
	if (!game) {
		return NextResponse.json(
			{ success: false, error: "Game not found" },
			{ status: 404 },
		);
	}
	const id = game.id;

	// If player is bot return error
	try {
		const player = await prisma.player.findFirst({
			where: {
				gameId: id,
				color: color,
			},
		});

		if (!player) {
			return NextResponse.json(
				{ success: false, error: "Player not found in game" },
				{ status: 404 },
			);
		}

		if (player.isBot) {
			return NextResponse.json(
				{ success: false, error: "Bots cannot leave the game" },
				{ status: 400 },
			);
		}

		// Update player to "Waiting..."
		await prisma.player.update({
			where: {
				id: player.id,
			},
			data: {
				name: "Waiting...",
			},
		});

		// Notify via WebSocket
		const io = GetSocketServer();
		io.to(`game-${id}`).emit("game-update");

		return NextResponse.json({ success: true });
	} catch (error) {
		return NextResponse.json(
			{
				success: false,
				error: "An error occurred while leaving the game",
			},
			{ status: 500 },
		);
	}
}
