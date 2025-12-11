/**
 * API Route: /api/snake/game
 * Správa Snake hier (CRUD operácie)
 * 
 * @author Igor Lacko
 */

import { NextResponse } from "next/server";
import {
	createGame,
	getGame,
	updateGameScore,
	finishGame,
	pauseGame,
	deleteGame,
	updatePlayerStats,
	deleteGameState,
} from "~/lib/db/snake";
import type { CreateGameRequest, UpdateGameRequest } from "~/lib/types/snake";

/**
 * POST - Vytvorenie novej hry
 * Body: { playerName, gameType, level }
 */
export async function POST(request: Request) {
	try {
		const body = (await request.json()) as CreateGameRequest;
		
		console.log('[API] POST /api/snake/game - Received body:', JSON.stringify(body));

		// Validácia
		if (!body.playerName || !body.gameType || !body.level) {
			return NextResponse.json(
				{ success: false, error: "Missing required fields: playerName, gameType, level" },
				{ status: 400 },
			);
		}

		if (body.level < 1 || body.level > 10) {
			return NextResponse.json(
				{ success: false, error: "Level must be between 1 and 10" },
				{ status: 400 },
			);
		}

		// Vytvorenie hry
		const game = await createGame(
			body.playerName,
			body.gameType,
			body.level,
		);
		
		console.log('[API] Created game:', JSON.stringify({ id: game.id, gameType: game.gameType, level: game.level }));

		return NextResponse.json({
			success: true,
			data: game,
		});
	} catch (error) {
		console.error("Error creating game:", error);
		return NextResponse.json(
			{ success: false, error: "Failed to create game" },
			{ status: 500 },
		);
	}
}

/**
 * GET - Získanie hry podľa ID
 * Query: ?gameId=xxx
 */
export async function GET(request: Request) {
	try {
		const { searchParams } = new URL(request.url);
		const gameId = searchParams.get("gameId");

		if (!gameId) {
			return NextResponse.json(
				{ success: false, error: "Missing gameId parameter" },
				{ status: 400 },
			);
		}

		const game = await getGame(gameId);

		if (!game) {
			return NextResponse.json(
				{ success: false, error: "Game not found" },
				{ status: 404 },
			);
		}
		
		console.log('[API] GET /api/snake/game - Returning game:', JSON.stringify({ id: game.id, gameType: game.gameType, level: game.level }));

		return NextResponse.json({
			success: true,
			data: game,
		});
	} catch (error) {
		console.error("Error fetching game:", error);
		return NextResponse.json(
			{ success: false, error: "Failed to fetch game" },
			{ status: 500 },
		);
	}
}

/**
 * PATCH - Aktualizácia hry
 * Body: { gameId, score?, status?, finishedAt? }
 */
export async function PATCH(request: Request) {
	try {
		const body = (await request.json()) as UpdateGameRequest & {
			gameId: string;
			playerName?: string;
			gameType?: string;
		};

		if (!body.gameId) {
			return NextResponse.json(
				{ success: false, error: "Missing gameId" },
				{ status: 400 },
			);
		}

		let updatedGame;

	// Ak aktualizujeme len meno hráča
	if (body.playerName && !body.status && body.score === undefined) {
		const game = await getGame(body.gameId);
		if (!game) {
			return NextResponse.json(
				{ success: false, error: "Game not found" },
				{ status: 404 },
			);
		}
		
		// Aktualizujeme meno hráča v databáze
		updatedGame = await updatePlayerStats(body.playerName, game.score, game.gameType as any);
		
		// Aktualizujeme aj meno v zázname hry
		const { db } = await import("~/server/db");
		updatedGame = await db.snakeGame.update({
			where: { id: body.gameId },
			data: { playerName: body.playerName },
		});
	}
	// Ak je status FINISHED, dokončíme hru
	else if (body.status === "FINISHED" && body.score !== undefined) {
		updatedGame = await finishGame(body.gameId, body.score);

		// Aktualizujeme štatistiky hráča
		if (body.playerName && body.gameType) {
			await updatePlayerStats(
				body.playerName,
				body.score,
				body.gameType as any,
			);
		}

		// Zmažeme uložený stav hry
		await deleteGameState(body.gameId);
	}
	// Ak je status PAUSED
	else if (body.status === "PAUSED") {
		updatedGame = await pauseGame(body.gameId);
	}
	// Inak len aktualizujeme skóre
	else if (body.score !== undefined) {
		updatedGame = await updateGameScore(body.gameId, body.score);
	} else {
		return NextResponse.json(
			{ success: false, error: "Nothing to update" },
			{ status: 400 },
		);
	}		return NextResponse.json({
			success: true,
			data: updatedGame,
		});
	} catch (error) {
		console.error("Error updating game:", error);
		return NextResponse.json(
			{ success: false, error: "Failed to update game" },
			{ status: 500 },
		);
	}
}

/**
 * DELETE - Zmazanie hry
 * Query: ?gameId=xxx
 */
export async function DELETE(request: Request) {
	try {
		const { searchParams } = new URL(request.url);
		const gameId = searchParams.get("gameId");

		if (!gameId) {
			return NextResponse.json(
				{ success: false, error: "Missing gameId parameter" },
				{ status: 400 },
			);
		}

		await deleteGame(gameId);

		return NextResponse.json({
			success: true,
		});
	} catch (error) {
		console.error("Error deleting game:", error);
		return NextResponse.json(
			{ success: false, error: "Failed to delete game" },
			{ status: 500 },
		);
	}
}
