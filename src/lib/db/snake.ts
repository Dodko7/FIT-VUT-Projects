/**
 * Database helper functions pre Snake game
 * 
 * @author Igor Lacko
 * @description Pomocné funkcie pre prácu s databázou (abstrakcia Prisma operácií)
 */

import { db } from "~/server/db";
import type {
	SnakeGameType,
	SnakeGameStatus,
	GamePosition,
	SnakeDirection,
} from "~/lib/types/snake";

// ============================================================================
// GAME OPERATIONS
// ============================================================================

/**
 * Vytvorí novú hru v databáze
 * 
 * @param playerName - Meno hráča
 * @param gameType - Typ hry (CLASSIC, BOX, CAMPAIGN)
 * @param level - Úroveň obtiažnosti (1-10)
 * @returns Vytvorená hra
 */
export async function createGame(
	playerName: string,
	gameType: SnakeGameType,
	level: number,
) {
	return await db.snakeGame.create({
		data: {
			playerName,
			gameType,
			level,
			status: "IN_PROGRESS",
		},
	});
}

/**
 * Získa hru podľa ID
 * 
 * @param gameId - ID hry
 * @returns Hra alebo null
 */
export async function getGame(gameId: string) {
	return await db.snakeGame.findUnique({
		where: { id: gameId },
		include: { gameState: true },
	});
}

/**
 * Aktualizuje skóre hry
 * 
 * @param gameId - ID hry
 * @param score - Nové skóre
 * @returns Aktualizovaná hra
 */
export async function updateGameScore(gameId: string, score: number) {
	return await db.snakeGame.update({
		where: { id: gameId },
		data: { score },
	});
}

/**
 * Označí hru ako dokončenú
 * 
 * @param gameId - ID hry
 * @param finalScore - Finálne skóre
 * @returns Aktualizovaná hra
 */
export async function finishGame(gameId: string, finalScore: number) {
	return await db.snakeGame.update({
		where: { id: gameId },
		data: {
			status: "FINISHED",
			score: finalScore,
			finishedAt: new Date(),
		},
	});
}

/**
 * Pauzne hru
 * 
 * @param gameId - ID hry
 * @returns Aktualizovaná hra
 */
export async function pauseGame(gameId: string) {
	return await db.snakeGame.update({
		where: { id: gameId },
		data: { status: "PAUSED" },
	});
}

/**
 * Zmaže hru
 * 
 * @param gameId - ID hry
 */
export async function deleteGame(gameId: string) {
	await db.snakeGame.delete({
		where: { id: gameId },
	});
}

// ============================================================================
// GAME STATE OPERATIONS
// ============================================================================

/**
 * Uloží aktuálny stav hry
 * 
 * @param gameId - ID hry
 * @param snakePositions - Pozície hada
 * @param foodPosition - Pozícia jedla
 * @param direction - Smer pohybu
 * @param speed - Rýchlosť hry
 * @returns Uložený stav
 */
export async function saveGameState(
	gameId: string,
	snakePositions: GamePosition[],
	foodPosition: GamePosition,
	direction: SnakeDirection,
	speed: number,
) {
	return await db.snakeGameState.upsert({
		where: { gameId },
		create: {
			gameId,
			snakePositions: JSON.stringify(snakePositions),
			foodPosition: JSON.stringify(foodPosition),
			direction,
			speed,
		},
		update: {
			snakePositions: JSON.stringify(snakePositions),
			foodPosition: JSON.stringify(foodPosition),
			direction,
			speed,
		},
	});
}

/**
 * Načíta uložený stav hry
 * 
 * @param gameId - ID hry
 * @returns Stav hry alebo null
 */
export async function loadGameState(gameId: string) {
	return await db.snakeGameState.findUnique({
		where: { gameId },
	});
}

/**
 * Zmaže uložený stav hry
 * 
 * @param gameId - ID hry
 */
export async function deleteGameState(gameId: string) {
	try {
		await db.snakeGameState.delete({
			where: { gameId },
		});
	} catch (error) {
		// Stav neexistuje - ignorujeme
		console.log(`Game state for ${gameId} does not exist`);
	}
}

// ============================================================================
// PLAYER OPERATIONS
// ============================================================================

/**
 * Aktualizuje štatistiky hráča po dokončení hry
 * 
 * @param playerName - Meno hráča
 * @param score - Dosiahnuté skóre
 * @param gameType - Typ hry
 */
export async function updatePlayerStats(
	playerName: string,
	score: number,
	gameType: SnakeGameType,
) {
	const existingPlayer = await db.snakePlayer.findUnique({
		where: { playerName },
	});

	const gameTypeField =
		gameType === "CLASSIC" ? "gamesClassic"
		: gameType === "BOX" ? "gamesBox"
		: "gamesCampaign";

	await db.snakePlayer.upsert({
		where: { playerName },
		create: {
			playerName,
			totalGames: 1,
			totalScore: score,
			bestScore: score,
			bestScoreGameType: gameType,
			[gameTypeField]: 1,
			lastPlayedAt: new Date(),
		},
		update: {
			totalGames: { increment: 1 },
			totalScore: { increment: score },
			bestScore:
				!existingPlayer || score > existingPlayer.bestScore ?
					score
				:	undefined,
			bestScoreGameType:
				!existingPlayer || score > existingPlayer.bestScore ?
					gameType
				:	undefined,
			[gameTypeField]: { increment: 1 },
			lastPlayedAt: new Date(),
		},
	});
}

/**
 * Získa štatistiky hráča
 * 
 * @param playerName - Meno hráča
 * @returns Štatistiky hráča alebo null
 */
export async function getPlayerStats(playerName: string) {
	return await db.snakePlayer.findUnique({
		where: { playerName },
	});
}

/**
 * Získa všetkých hráčov
 * 
 * @param limit - Maximálny počet výsledkov
 * @returns Zoznam hráčov
 */
export async function getAllPlayers(limit = 50) {
	return await db.snakePlayer.findMany({
		orderBy: { bestScore: "desc" },
		take: limit,
	});
}

// ============================================================================
// LEADERBOARD OPERATIONS
// ============================================================================

/**
 * Získa leaderboard (top hry)
 * 
 * @param gameType - Filter podľa typu hry (voliteľné)
 * @param limit - Maximálny počet výsledkov
 * @returns Zoznam top hier
 */
export async function getLeaderboard(
	gameType?: SnakeGameType,
	limit = 10,
) {
	return await db.snakeGame.findMany({
		where: {
			status: "FINISHED",
			...(gameType && { gameType }),
		},
		orderBy: { score: "desc" },
		take: limit,
	});
}

/**
 * Získa top hráčov (podľa best score)
 * 
 * @param limit - Maximálny počet výsledkov
 * @returns Zoznam top hráčov
 */
export async function getTopPlayers(limit = 10) {
	return await db.snakePlayer.findMany({
		orderBy: { bestScore: "desc" },
		take: limit,
	});
}

/**
 * Získa hry konkrétneho hráča
 * 
 * @param playerName - Meno hráča
 * @param limit - Maximálny počet výsledkov
 * @returns Zoznam hier hráča
 */
export async function getPlayerGames(playerName: string, limit = 10) {
	return await db.snakeGame.findMany({
		where: {
			playerName,
			status: "FINISHED",
		},
		orderBy: { createdAt: "desc" },
		take: limit,
	});
}
