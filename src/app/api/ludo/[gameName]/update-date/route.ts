import { NextResponse, type NextRequest } from "next/server";
import { db as prisma } from "~/server/db";

export async function POST(
	request: NextRequest,
	{ params }: { params: Promise<{ gameName: string }> },
): Promise<NextResponse> {
	const gameName = (await params).gameName;

	if (!gameName) {
		return NextResponse.json(
			{ success: false, error: "Invalid game name" },
			{ status: 400 },
		);
	}

	try {
		await prisma.game.update({
			where: { name: gameName },
			data: { lastPlayed: new Date() },
		});
		return NextResponse.json({ success: true });
	} catch (error) {
		return NextResponse.json(
			{ success: false, error: "Internal server error" },
			{ status: 500 },
		);
	}
}
