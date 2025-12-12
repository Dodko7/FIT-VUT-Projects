import { NextRequest, NextResponse } from "next/server";
import { db as prisma } from "~/server/db";

/**
 * Rolls the dice for a specific game.
 * TODO
 */
export async function POST(
    request: NextRequest,
    { params }: { params: Promise<{ gameName: string }> }
): Promise<NextResponse> {
    try {
        const gameName = (await params).gameName;

        return NextResponse.json({
            success: true,
        });
    } catch (error) {
        return NextResponse.json(
            {
                success: false,
                error: "An unknown error occurred.",
            },
            { status: 500 }
        );
    }
}