/**
 * Snake Game Context - Správa stavu hry počas session
 * 
 * @author Igor Lacko
 * @description Poskytuje globálny stav pre nastavenia Snake hry
 *              Ukladá gameType, level, currentGameId a playerName
 */

"use client";

import { createContext, useContext, useState, type ReactNode } from "react";
import type { SnakeGameType } from "~/lib/types/snake";

// ============================================================================
// TYPES
// ============================================================================

interface SnakeGameContextType {
	// Herné nastavenia
	gameType: SnakeGameType;
	level: number;
	
	// Aktuálna hra
	currentGameId: string | null;
	playerName: string;
	
	// Settery
	setGameType: (type: SnakeGameType) => void;
	setLevel: (level: number) => void;
	setCurrentGameId: (id: string | null) => void;
	setPlayerName: (name: string) => void;
	
	// Utility funkcie
	resetSettings: () => void;
	resetAll: () => void;
}

// ============================================================================
// CONTEXT
// ============================================================================

const SnakeGameContext = createContext<SnakeGameContextType | undefined>(
	undefined,
);

// ============================================================================
// PROVIDER
// ============================================================================

/**
 * Provider pre Snake Game Context
 * Obaľuje všetky Snake routes a poskytuje im prístup k hernému stavu
 */
export function SnakeGameProvider({ children }: { children: ReactNode }) {
	// Herné nastavenia (default hodnoty)
	const [gameType, setGameType] = useState<SnakeGameType>("CLASSIC");
	const [level, setLevel] = useState<number>(5);
	
	// Aktuálna hra
	const [currentGameId, setCurrentGameId] = useState<string | null>(null);
	const [playerName, setPlayerName] = useState<string>("");

	/**
	 * Reset nastavení na default hodnoty (ale ponechá playerName a gameId)
	 */
	const resetSettings = () => {
		setGameType("CLASSIC");
		setLevel(5);
	};

	/**
	 * Kompletný reset - vráti všetko na default
	 */
	const resetAll = () => {
		setGameType("CLASSIC");
		setLevel(5);
		setCurrentGameId(null);
		setPlayerName("");
	};

	return (
		<SnakeGameContext.Provider
			value={{
				gameType,
				level,
				currentGameId,
				playerName,
				setGameType,
				setLevel,
				setCurrentGameId,
				setPlayerName,
				resetSettings,
				resetAll,
			}}
		>
			{children}
		</SnakeGameContext.Provider>
	);
}

// ============================================================================
// HOOK
// ============================================================================

/**
 * Custom hook pre prístup k Snake Game Context
 * 
 * @throws Error ak sa použije mimo SnakeGameProvider
 * @returns Snake game context value
 * 
 * @example
 * const { gameType, setGameType, level, setLevel } = useSnakeGame();
 */
export function useSnakeGame() {
	const context = useContext(SnakeGameContext);
	
	if (context === undefined) {
		throw new Error(
			"useSnakeGame must be used within SnakeGameProvider. " +
			"Make sure your component is wrapped in <SnakeGameProvider>.",
		);
	}
	
	return context;
}
