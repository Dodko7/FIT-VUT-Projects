/**
 * TypeScript type definitions pre Snake game
 * 
 * Definície typov pre frontend Snake hry
 * Obsahuje game types, pozície, databázové modely, API requesty/responsy
 * 
 * @author Jozef Ondrejicka
 */

// ============================================================================
// GAME TYPES
// ============================================================================

/**
 * Typy Snake hier
 */
export type SnakeGameType = "CLASSIC" | "BOX" | "CAMPAIGN";

/**
 * Stavy hry
 */
export type SnakeGameStatus = "IN_PROGRESS" | "FINISHED" | "PAUSED";

/**
 * Smery pohybu hada
 */
export type SnakeDirection = "UP" | "DOWN" | "LEFT" | "RIGHT";

// ============================================================================
// POSITION TYPES
// ============================================================================

/**
 * Pozícia na hracej ploche (grid)
 */
export interface GamePosition {
	x: number;
	y: number;
}

/**
 * Kompletný stav hry pre save/load
 */
export interface GameStateData {
	snakePositions: GamePosition[];
	foodPosition: GamePosition;
	direction: SnakeDirection;
	speed: number;
}

// ============================================================================
// DATABASE MODELS (Frontend representations)
// ============================================================================

/**
 * Snake hra (frontend reprezentácia)
 */
export interface SnakeGame {
	id: string;
	playerName: string;
	gameType: SnakeGameType;
	level: number;
	score: number;
	status: SnakeGameStatus;
	createdAt: Date;
	finishedAt?: Date | null;
}

/**
 * Stav hry (frontend reprezentácia)
 */
export interface SnakeGameState {
	id: string;
	gameId: string;
	snakePositions: string; // JSON string
	foodPosition: string; // JSON string
	direction: string;
	speed: number;
	lastUpdated: Date;
}

/**
 * Štatistiky hráča (frontend reprezentácia)
 */
export interface SnakePlayer {
	id: string;
	playerName: string;
	totalGames: number;
	totalScore: number;
	bestScore: number;
	bestScoreGameType: SnakeGameType;
	gamesClassic: number;
	gamesBox: number;
	gamesCampaign: number;
	lastPlayedAt: Date;
	createdAt: Date;
}

// ============================================================================
// LEADERBOARD & STATS
// ============================================================================

/**
 * Záznam pre leaderboard
 */
export interface LeaderboardEntry {
	position: number;
	playerName: string;
	score: number;
	level: number;
	gameType: SnakeGameType;
	date: Date;
	gameId: string;
}

/**
 * Štatistiky hráča (rozšírené)
 */
export interface PlayerStats extends SnakePlayer {
	averageScore: number;
	winRate: number;
	recentGames: SnakeGame[];
}

// ============================================================================
// API REQUEST/RESPONSE TYPES
// ============================================================================

/**
 * Request pre vytvorenie novej hry
 */
export interface CreateGameRequest {
	playerName: string;
	gameType: SnakeGameType;
	level: number;
}

/**
 * Request pre update hry
 */
export interface UpdateGameRequest {
	score?: number;
	status?: SnakeGameStatus;
	finishedAt?: Date;
}

/**
 * Request pre uloženie stavu hry
 */
export interface SaveGameStateRequest {
	gameId: string;
	snakePositions: GamePosition[];
	foodPosition: GamePosition;
	direction: SnakeDirection;
	speed: number;
}

/**
 * Response pre načítanie stavu hry
 */
export interface LoadGameStateResponse {
	gameState: GameStateData;
	game: SnakeGame;
}

// ============================================================================
// RESULT TYPES
// ============================================================================

/**
 * Generický Result type pre API odpovede
 */
export type Result<T = void> =
	| {
			success: true;
			data: T;
	  }
	| {
			success: false;
			error: string;
	  };

/**
 * Result type pre operácie bez návratovej hodnoty
 */
export type SimpleResult =
	| {
			success: true;
	  }
	| {
			success: false;
			error: string;
	  };

// ============================================================================
// GAME SETTINGS
// ============================================================================

/**
 * Herné nastavenia
 */
export interface GameSettings {
	gameType: SnakeGameType;
	level: number;
	gridSize: number;
	initialSpeed: number;
}

/**
 * Default nastavenia
 */
export const DEFAULT_GAME_SETTINGS: GameSettings = {
	gameType: "CLASSIC",
	level: 5,
	gridSize: 20,
	initialSpeed: 100,
};
