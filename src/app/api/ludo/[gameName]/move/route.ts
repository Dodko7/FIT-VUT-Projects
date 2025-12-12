import { NextResponse, type NextRequest } from "next/server";
import { GetHomeFromColor, GetStartFromColor } from "~/lib/ludo/utils";
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
		const pawn = await prisma.pawn.findFirst({
			where: {
				id: pawnId,
				player: {
					game: {
						name: gameName,
					},
				},
			},
		});

		if (!pawn) {
			return NextResponse.json(
				{
					success: false,
					error: "Pawn not found or does not belong to the game",
				},
				{ status: 404 },
			);
		}

		// Check if the new position is occupied by another pawn
		const otherPawn = await prisma.pawn.findFirst({
			where: {
				position: position,
				NOT: {
					id: pawnId,
				},
			},
		});

		// Get start position for the pawn's color
		if (otherPawn) {
			const startPositions = GetStartFromColor(otherPawn.color);

			// Move the other pawn to its start position (but find the first free one)
			for (const startPos of startPositions) {
				const isOccupied = await prisma.pawn.findFirst({
					where: {
						position: startPos,
					},
				});

				if (!isOccupied) {
					await prisma.pawn.update({
						where: { id: otherPawn.id },
						data: { position: startPos },
					});
					break;
				}
			}
		}

		// Move the pawn to the new position and check for home
		const homePositions = GetHomeFromColor(pawn.color);
		const isHome = homePositions.includes(position);

		await prisma.pawn.update({
			where: { id: pawnId },
			data: {
				position: position,
				inHome: isHome,
			},
		});

		return NextResponse.json({ success: true });
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
