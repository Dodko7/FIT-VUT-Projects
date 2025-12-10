/**
 * @brief API for pacman levels 
 */

import { db } from "~/server/db";
import { NextResponse } from "next/server";

/**
 * @brief get levels from db
 * @returns JSON levels
 */
export async function GET() {
	try {
		const levels = await db.pacmanLevel.findMany();
		return NextResponse.json(levels);
	} catch (error) {
		return NextResponse.json({ error: "Failed to fetch levels" }, { status: 500 });
	}
}
