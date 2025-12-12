import { NextRequest, NextResponse } from "next/server";
import { NEXT_COLOR } from "~/lib/ludo/constants";
import PawnSpotHighlight from "~/lib/ludo/enum/pawn-spot-highlight";
import type { AvaliablePawnMoves, DiceRoll, HighlightedPawnSpot, RollDiceResult } from "~/lib/ludo/types";
import { GetAvailableMoves } from "~/lib/ludo/utils";
import { db as prisma } from "~/server/db";

/**
 * Rolls the dice for a specific game.
 */
export async function POST(
	request: NextRequest,
	{ params }: { params: Promise<{ gameName: string }> },
): Promise<NextResponse> {
	try {
		const gameName = (await params).gameName;

		// Find the game
		const game = await prisma.game.findFirst({
			where: { name: gameName },
		});

		if (!game) {
			return NextResponse.json(
				{
					success: false,
					error: `Game with name ${gameName} not found.`,
				},
				{ status: 404 },
			);
		}

		// Generate random dice roll
		const diceRoll = Math.floor(Math.random() * 6) + 1;

		// Get available pawns for the current player

		// 1. Get all players (needed for highlights later)
		const players = await prisma.player.findMany({
			where: { gameId: game.id },
			include: { pawns: true },
		});

		// And all colors
		const colorsInGame = players.map((p) => p.color);

		// 2. Find current player
		const currentPlayer = players.find((p) => p.color === game.turn)!;

		/**
		 * 3. Get available moves for each pawn and build highlights
		 *  - This means that the pawn has at least one valid move
		 */
		const allPawns = players.flatMap((p) => p.pawns);
		let moves: AvaliablePawnMoves[] = [];
		let pawns: HighlightedPawnSpot[] = [];
		for (const pawn of currentPlayer.pawns || []) {
			const availableMoves = GetAvailableMoves(
				pawn,
				allPawns,
				currentPlayer.color,
				diceRoll,
			);

			if (availableMoves.length > 0) {
				pawns.push({
					position: pawn.position,
					highlight: PawnSpotHighlight.AVALIABLE_PAWN,
				});
				moves.push({
					pawnId: pawn.id,
					moves: availableMoves,
				});
			}
		}

		// Update turn straight away if no moves are available
		if (pawns.length === 0) {
			let nextColor = NEXT_COLOR[game.turn];
			while (!colorsInGame.includes(nextColor)) {
				nextColor = NEXT_COLOR[nextColor];
			}
			await prisma.game.update({
				where: { id: game.id },
				data: { turn: nextColor },
			});
		}

        const res: RollDiceResult = {
            diceNumber: diceRoll as DiceRoll,
            avaliablePawns: pawns,
            avaliableMoves: moves,
            refetch: pawns.length === 0,
        }
		return NextResponse.json({
			success: true,
			value: res,
		});
	} catch (error) {
		return NextResponse.json(
			{
				success: false,
				error: "An unknown error occurred.",
			},
			{ status: 500 },
		);
	}
}
