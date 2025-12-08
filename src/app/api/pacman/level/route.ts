import { PrismaClient } from "@prisma/client";
import { NextResponse } from "next/server";

const prisma = new PrismaClient();

export async function GET() {
	try {
		const levels = await prisma.pacmanLevel.findMany();
		return NextResponse.json(levels);
	} catch (error) {
		return NextResponse.json(
			{ error: "Failed to fetch levels" },
			{ status: 500 },
		);
	}
}
