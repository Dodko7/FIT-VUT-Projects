import type { NextRequest } from "next/server";
import { db as prisma } from "~/server/db";

export async function DELETE(request: NextRequest, { params }: { params: { gameId: string } }) {
    console.log("DELETE request received for game ID:", params.gameId);
    const { gameId } = (await params) as { gameId: string };
    const numberId = parseInt(gameId, 10);
    console.log("Parsed game ID as number:", numberId);
    if (isNaN(numberId)) {
        return new Response(`Invalid game ID: ${gameId}`, { status: 400 });
    }

    try {
        await prisma.game.delete({
            where: { id: numberId }
        });
        return new Response(null, { status: 204 });
    } catch (error) {
        return new Response(`Failed to delete game with ID ${gameId}: ${(error as Error).message}`, { status: 500 });
    }
}