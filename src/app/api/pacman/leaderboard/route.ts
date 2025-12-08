import { db } from "~/server/db";
import { NextResponse } from "next/server";

export async function GET(request: Request) {
    const { searchParams } = new URL(request.url);
    const levelIdStr = searchParams.get("levelId");
    const levelId = levelIdStr ? parseInt(levelIdStr) : undefined;

    const scores = await db.pacmanScore.findMany({
        where: levelId ? { levelId: levelId } : {},
        orderBy: { score: "desc" },
        take: 10,
    });
    return NextResponse.json(scores);
}

export async function POST(request: Request) {
    const body = await request.json();
    const { playerName, score, levelId } = body;
    
    if (!playerName || typeof(score) !== "number" || !levelId) {
        return NextResponse.json({ error: "Invalid data"}, { status: 400 });
    }

    const newScore = await db.pacmanScore.create({
        data: {
            playerName,
            score, 
            levelId,
        },
    });

    return NextResponse.json(newScore);
}