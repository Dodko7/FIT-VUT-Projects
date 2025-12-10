/**
 * @brief API for pacman leaderboard
 */

import { db } from "~/server/db";
import { NextResponse } from "next/server";

/**
 * @brief get top 5 scores for certain level
 * @returns JSON response with scores
 */
export async function GET(request: Request) {
    try {
        const { searchParams } = new URL(request.url);
        const levelIdStr = searchParams.get("levelId");
        const levelId = levelIdStr ? parseInt(levelIdStr) : undefined;

        const scores = await db.pacmanScore.findMany({
            where: levelId ? { levelId: levelId } : {},
            orderBy: { score: "desc" },
            take: 5,
        });
        return NextResponse.json(scores);
    } catch(error) {
        return NextResponse.json({ error: "Failed to fetch levels" }, { status: 400 });
    }
}

/**
 * @brief saves a new score to the db 
 * @param request object containing playerName, score, levelId
 * @returns JSON response with new score
 */
export async function POST(request: Request) {
    try {
        const body = await request.json();
        const { playerName, score, levelId } = body;
        
        const newScore = await db.pacmanScore.create({
            data: {
                playerName,
                score, 
                levelId,
            },
        });

        return NextResponse.json(newScore);
    } catch(error) {
        return NextResponse.json({ error: "Invalid data"}, { status: 400 });
    }
}