import { NextRequest, NextResponse } from "next/server";
import { db as prisma } from "~/server/db";

/**
 * Rolls the dice for a specific game.
 */
export async function POST(
    request: NextRequest,
    { params }: { params: Promise<{ id: string }> }
): Promise<NextResponse> {
    try {
        const id = parseInt((await params).id, 10);

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