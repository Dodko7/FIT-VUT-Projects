import { NextResponse } from "next/server";
import { db as prisma } from "~/server/db";

/**
 * On game load. Returns the full game state.
 * @returns The full game state.
 */
export async function GET(): Promise<NextResponse> {
    try {
        // Holy Flutter
        const current = await prisma.currentGame.findFirst({
            include: {
                game: {
                    include: {
                        players: {
                            include: {
                                pawns: true,
                            }
                        }
                    }
                }
            }
        });

        if (!current || !current.game) {
            return NextResponse.json(
                {
                    success: false,
                    error: "No active game found!",
                },
                { status: 404 },
            );
        }

        const game = current.game;

        if (!game) {
            return NextResponse.json(
                {
                    success: false,
                    error: "No active game found!",
                },
                { status: 404 },
            );
        }

        return NextResponse.json({
            success: true,
            value: game,
        });
    } catch (error) {
        return NextResponse.json(
            {
                success: false,
                error: "An unknown error occurred while loading the game.",
            },
            { status: 500 }
        );
    }
}