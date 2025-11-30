/**
 * API Route: /api/snake/game-state
 * Správa stavu Snake hry (Save/Load)
 * 
 * @author Igor Lacko
 */

import { NextResponse } from "next/server";
import {
	saveGameState,
	loadGameState,
	deleteGameState,
	getGame,
} from "~/lib/db/snake";
import type { SaveGameStateRequest } from "~/lib/types/snake";

/**
 * POST - Uloženie stavu hry
 * Body: { gameId, snakePositions, foodPosition, direction, speed }
 */
export async function POST(request: Request) {
	try {
		const body = (await request.json()) as SaveGameStateRequest;

		// Validácia
		if (
			!body.gameId ||
			!body.snakePositions ||
			!body.foodPosition ||
			!body.direction
		) {
			return NextResponse.json(
				{
					success: false,
					error: "Missing required fields: gameId, snakePositions, foodPosition, direction",
				},
				{ status: 400 },
			);
		}

		// Uloženie stavu
		const gameState = await saveGameState(
			body.gameId,
			body.snakePositions,
			body.foodPosition,
			body.direction,
			body.speed ?? 100,
		);

		return NextResponse.json({
			success: true,
			data: gameState,
		});
	} catch (error) {
		console.error("Error saving game state:", error);
		return NextResponse.json(
			{ success: false, error: "Failed to save game state" },
			{ status: 500 },
		);
	}
}

/**
 * GET - Načítanie stavu hry
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

		// Načítame stav aj hru
		const [gameState, game] = await Promise.all([
			loadGameState(gameId),
			getGame(gameId),
		]);

		if (!gameState || !game) {
			return NextResponse.json(
				{ success: false, error: "Game state not found" },
				{ status: 404 },
			);
		}

		// Parsujeme JSON data
		const response = {
			gameState: {
				snakePositions: JSON.parse(gameState.snakePositions),
				foodPosition: JSON.parse(gameState.foodPosition),
				direction: gameState.direction,
				speed: gameState.speed,
			},
			game: {
				id: game.id,
				playerName: game.playerName,
				gameType: game.gameType,
				level: game.level,
				score: game.score,
				status: game.status,
				createdAt: game.createdAt,
				finishedAt: game.finishedAt,
			},
		};

		return NextResponse.json({
			success: true,
			data: response,
		});
	} catch (error) {
		console.error("Error loading game state:", error);
		return NextResponse.json(
			{ success: false, error: "Failed to load game state" },
			{ status: 500 },
		);
	}
}

/**
 * DELETE - Zmazanie stavu hry
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

		await deleteGameState(gameId);

		return NextResponse.json({
			success: true,
		});
	} catch (error) {
		console.error("Error deleting game state:", error);
		return NextResponse.json(
			{ success: false, error: "Failed to delete game state" },
			{ status: 500 },
		);
	}
}
