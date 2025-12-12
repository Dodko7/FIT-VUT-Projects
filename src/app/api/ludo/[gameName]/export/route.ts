import { NextResponse } from "next/server";
import { db as prisma } from "~/server/db";

export async function GET(
    request: Request,
	{ params }: { params: Promise<{ gameName: string }> },
): Promise<NextResponse> {
    const gameName = (await params).gameName;
    const game = await prisma.game.findUnique({
        where: { name: gameName },
        include: {
            players: {
                include: {
                    pawns: true,
                },
            },
        },
    });

    const { id, ...without } = game ?? {};

    return NextResponse.json(without);
}