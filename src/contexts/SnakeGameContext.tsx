/**
 * Snake Game Context - Správa stavu hry počas session
 * 
 * Poskytuje globálny stav pre nastavenia Snake hry
 * Ukladá gameType, level, currentGameId a playerName
 * Zachováva nastavenia v localStorage pre perzistenciu naprieč reláciami
 * 
 * @author Jozef Ondrejicka
 */

"use client";

import { createContext, useContext, useState, useEffect, type ReactNode } from "react";
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
 * Pri inicializácii načíta nastavenia z localStorage
 */
export function SnakeGameProvider({ children }: { children: ReactNode }) {
	// Load initial values from localStorage
	const [gameType, setGameTypeState] = useState<SnakeGameType>(() => {
		if (typeof window !== "undefined") {
			const saved = localStorage.getItem("snake_gameType");
			return (saved as SnakeGameType) || "CLASSIC";
		}
		return "CLASSIC";
	});
	
	const [level, setLevelState] = useState<number>(() => {
		if (typeof window !== "undefined") {
			const saved = localStorage.getItem("snake_level");
			return saved ? parseInt(saved, 10) : 5;
		}
		return 5;
	});
	
	// Aktuálna hra
	const [currentGameId, setCurrentGameId] = useState<string | null>(null);
	const [playerName, setPlayerName] = useState<string>("");

	// Persist gameType to localStorage
	const setGameType = (type: SnakeGameType) => {
		setGameTypeState(type);
		if (typeof window !== "undefined") {
			localStorage.setItem("snake_gameType", type);
		}
	};

	// Persist level to localStorage
	const setLevel = (newLevel: number) => {
		setLevelState(newLevel);
		if (typeof window !== "undefined") {
			localStorage.setItem("snake_level", newLevel.toString());
		}
	};

	/**
	 * Resetuje herné nastavenia na predvolené hodnoty
	 * Ponechá playerName a currentGameId nezmenené
	 */
	const resetSettings = () => {
		setGameType("CLASSIC");
		setLevel(5);
	};

	/**
	 * Kompletný reset - vráti všetko na predvolené hodnoty
	 * Vymaže aj playerName a currentGameId
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
 * @returns Hodnoty a funkcie Snake game contextu
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
