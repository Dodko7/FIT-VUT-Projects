import type { Color } from "@prisma/client";
import { NextResponse, type NextRequest } from "next/server";
import { db as prisma } from "~/server/db";

/**
 * Called when a player leaves a game.
 * @param Request The incoming request.
 * @param params Contains the game id.
 * @returns A response indicating the result of the leave operation.
 */
export async function POST(
	Request: NextRequest,
	{ params }: { params: Promise<{ id: string }> },
): Promise<NextResponse> {
	// Parse id
	const id = parseInt((await params).id, 10);
	if (isNaN(id)) {
		return NextResponse.json(
			{ success: false, error: "Invalid game ID" },
			{ status: 400 },
		);
	}

	// Parse color from body
	const body = await Request.json();
	const color: Color = body.color;

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
