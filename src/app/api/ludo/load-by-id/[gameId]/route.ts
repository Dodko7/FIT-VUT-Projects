import { NextRequest, NextResponse } from "next/server";
import { db as prisma } from "~/server/db";

export async function GET(request: NextRequest, { params }: { params: { gameId: string } }): Promise<NextResponse<unknown>> {
    console.log("Received request to load game by ID.");
    const { gameId } = (await params) as { gameId: string };
    const numberId = parseInt(gameId, 10);
    if (isNaN(numberId)) {
        return NextResponse.json({ success: false, error: `Invalid game ID: ${gameId}` }, { status: 400 });
    }

    console.log(`Loading game with ID: ${gameId}`);

    try {
        const game = await prisma.game.findUnique({
            where: { id: numberId }
        });

        if (!game) {
            return NextResponse.json({ success: false, error: `Game with ID ${gameId} not found.` }, { status: 404 });
        }

        // Set as the current active game
        await prisma.currentGame.deleteMany({});
        await prisma.currentGame.create({
            data: { gameId: numberId }
        });

        return NextResponse.json({ success: true });
    } catch (error) {
        return NextResponse.json({ success: false, error: `Failed to load game with ID ${gameId}: ${(error as Error).message}` }, { status: 500 });
    }
}