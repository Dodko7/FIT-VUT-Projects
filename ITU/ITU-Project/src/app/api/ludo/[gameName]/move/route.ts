import { NextResponse, type NextRequest } from "next/server";
import { NEXT_COLOR } from "~/lib/ludo/constants";
import { DoBotTurns, MovePawn } from "~/lib/ludo/movement";
import type { MovePawnResult, TypedResult } from "~/lib/ludo/types";
import {
	GetAvailableMoves,
	GetHomeFromColor,
	GetStartFromColor,
} from "~/lib/ludo/utils";
import { db as prisma } from "~/server/db";

/**
 * Moves a pawn in a Ludo game.
 * @param request Contains the pawn ID and the position to move to.
 * @param params Contains the game name.
 * @returns A response indicating success or failure.
 */
export async function POST(
	request: NextRequest,
	{ params }: { params: Promise<{ gameName: string }> },
): Promise<NextResponse> {
	const gameName = (await params).gameName;
	if (!gameName) {
		return NextResponse.json(
			{ success: false, error: "Invalid game name" },
			{ status: 400 },
		);
	}

	const { pawnId, position } = await request.json();
	if (typeof pawnId !== "number" || typeof position !== "number") {
		return NextResponse.json(
			{ success: false, error: "Invalid request data" },
			{ status: 400 },
		);
	}

	// Position can't be < 0 or > 59
	if (position < 0 || position > 59) {
		return NextResponse.json(
			{ success: false, error: "Invalid position" },
			{ status: 400 },
		);
	}

	try {
		/**
		 * We have to do the following:
		 * 1. Check if the pawn exists and belongs to the game.
		 * 2. Move the pawn to the new position.
		 * 3. If another pawn is at the new position, move it to it's start.
		 * 4. If the new pawn position is a home position, set the ishome flag.
		 */
		await MovePawn(pawnId, position);

		// Get the pawn to find the player
		const pawn = await prisma.pawn.findUnique({
			where: { id: pawnId },
		});

		if (!pawn) {
			return NextResponse.json(
				{ success: false, error: "Pawn not found after move" },
				{ status: 500 },
			);
		}

		// If the game is not over (player has all pawns in home) and no extra turn, switch turn
		const game = await prisma.game.findFirst({
			where: {
				name: gameName,
			},
		});

		if (!game) {
			return NextResponse.json(
				{ success: false, error: "Game not found" },
				{ status: 404 },
			);
		}

		const playerPawns = await prisma.pawn.findMany({
			where: {
				playerId: pawn.playerId,
				gameId: game.id,
			},
		});

		const allInHome = playerPawns.every((p) => p.inHome);
		if (!allInHome && !game.extraTurn) {
			// This will do all bot turns and switch to next human
			await DoBotTurns(gameName);
		} else if (allInHome) {
			const announcement: MovePawnResult = {
				isOver: true,
			};

			const res: TypedResult<MovePawnResult> = {
				success: true,
				value: announcement,
			};

			// Update the game too in case the client refetches
			const game = await prisma.game.findFirst({
				where: {
					name: gameName,
				},
			});

			if (game) {
				await prisma.game.update({
					where: { id: game.id },
					data: { over: true },
				});
			}

			return NextResponse.json(res);
		} else if (game.extraTurn) {
			// Just toggle
			await prisma.game.update({
				where: { id: game.id },
				data: { extraTurn: false },
			});
		}

		return NextResponse.json({ success: true, value: { isOver: false } });
	} catch (error) {
		return NextResponse.json(
			{
				success: false,
				error: "An error occurred while moving the pawn",
			},
			{ status: 500 },
		);
	}
}
