import { db as prisma } from "~/server/db";
import { GetAvailableMoves, GetHomeFromColor, GetStartFromColor } from "./utils";
import { NEXT_COLOR } from "./constants";

/**
 * Moves a pawn in a Ludo game.
 * @param pawnId The ID of the pawn to move.
 * @param gameName The name of the game.
 * @param newPosition The new position to move the pawn to.
 */
export async function MovePawn(
	pawnId: number,
	newPosition: number,
) {
    const pawn = await prisma.pawn.findUnique({
        where: { id: pawnId },
    });

    if (!pawn) {
        throw new Error(`Pawn with ID ${pawnId} not found`);
    }

    const gameId = pawn.gameId;
	// Check if the new position is occupied by another pawn in the same game
	const occupyingPawn = await prisma.pawn.findFirst({
		where: {
			position: newPosition,
            gameId: gameId,
			NOT: {
				id: pawnId,
			},
		},
	});

    // Move the other pawn to its starting position if occupied
    if (occupyingPawn) {
        const startPositions = GetStartFromColor(occupyingPawn.color);
        for (const pos of startPositions) {
            const isOccupied = await prisma.pawn.findFirst({
                where: {
                    position: pos,
                },
            });
            if (!isOccupied) {
                await prisma.pawn.update({
                    where: { id: occupyingPawn.id },
                    data: { position: pos },
                });
                break;
            }
        }
    }

    const homePositions = GetHomeFromColor(pawn.color);
    const isInHome = homePositions.includes(newPosition);

    await prisma.pawn.update({
        where: { id: pawnId },
        data: {
            position: newPosition,
            inHome: isInHome,
        },
    });
}

export async function DoBotTurns(gameName: string): Promise<void> {
    // Fetch game
    const game = await prisma.game.findFirst({
        where: { name: gameName },
        include: {
            players: {
                include: {
                    pawns: true,
                },
            },
        },
    });

    if (!game) {
        throw new Error(`Game with name ${gameName} not found`);
    }

    // Current turn
    let turn = game.turn;

    // All players
    const players = game.players;
    const colorsInGame = players.map((p) => p.color);

    // All pawns in game
    const allPawns = players.flatMap((p) => p.pawns);

    // Loop until the next human player (turn is still set to the last human player)
    while (true) {
        let nextColor = NEXT_COLOR[turn];
        while (!colorsInGame.includes(nextColor)) {
            nextColor = NEXT_COLOR[nextColor];
        }

        turn = nextColor;

        // Get player with this color
        const nextPlayer = players.find((p) => p.color === nextColor);
        // Found the next human player
        if (nextPlayer && !nextPlayer.isBot) {
            break;
        }

        // BOT
        else if (nextPlayer && nextPlayer.isBot) {
            const botPawns = nextPlayer.pawns;
            const diceRoll = Math.floor(Math.random() * 6) + 1;

            // Available moves
            let moves = [];
            for (const pawn of botPawns) {
                const availableMoves = GetAvailableMoves(
                    pawn,
                    allPawns,
                    nextPlayer.color,
                    diceRoll,
                );

                if (availableMoves.length > 0) {
                    moves.push({
                        pawn: pawn,
                        moves: availableMoves,
                    });
                }
            }

            // If no moves, continue to next player
            if (moves.length === 0) {
                continue;
            }

            // Pick a random available pawn and move
            const randomIndex = Math.floor(Math.random() * moves.length);
            const selectedMove = moves[randomIndex];
            const pawnToMove = selectedMove!.pawn;
            const possibleMoves = selectedMove!.moves;

            // Just in case
            if (possibleMoves.length === 0) {
                continue;
            }

            const moveIndex = Math.floor(Math.random() * possibleMoves.length);
            const moveTo = possibleMoves[moveIndex];

            // Move the pawn (this moves the bot and handles capturing)
            await MovePawn(pawnToMove.id, moveTo!.position);

            // Check if game over
            const allInHome = botPawns.every((p) => p.inHome);
            if (allInHome) {
                // Update game state to finished
                await prisma.game.update({
                    where: { id: game.id },
                    data: { over: true },
                });
                return;
            }

            // Bots do not get extra turns
        }
    }

    // Update turn to next human
    await prisma.game.update({
        where: { id: game.id },
        data: { turn: turn, extraTurn: false },
    });
}