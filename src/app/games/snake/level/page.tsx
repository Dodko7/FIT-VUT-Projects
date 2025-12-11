/**
 * Snake Level Selection Page
 * 
 * @author Igor Lacko
 * @description Výber úrovne obtiažnosti (1-10)
 *              Ukladá do Context a vytvorí novú hru v databáze
 */

"use client";

import { useState, useEffect } from "react";
import Link from "next/link";
import { useRouter } from "next/navigation";
import { useSnakeGame } from "~/contexts/SnakeGameContext";
import PlayerNameModal from "~/components/games/snake/PlayerNameModal";

export default function SnakeLevelPage() {
	const { level: contextLevel, setLevel: setContextLevel, gameType, playerName, setPlayerName, setCurrentGameId } = useSnakeGame();
	const router = useRouter();
	
	// Local state pre slider (synchronizovaný s context)
	const [level, setLevel] = useState(contextLevel);
	const [isCreatingGame, setIsCreatingGame] = useState(false);
	const [error, setError] = useState<string | null>(null);
	const [showNameModal, setShowNameModal] = useState(false);
	
	const minLevel = 1;
	const maxLevel = 10;

	// Synchronizuj local state s context pri načítaní
	useEffect(() => {
		setLevel(contextLevel);
	}, [contextLevel]);

	// Update context pri zmene levelu
	useEffect(() => {
		setContextLevel(level);
	}, [level, setContextLevel]);

	const handleKeyDown = (e: React.KeyboardEvent) => {
		if (e.key === "ArrowLeft" || e.key === "<" || e.key === ",") {
			e.preventDefault();
			setLevel((prev) => Math.max(minLevel, prev - 1));
		} else if (e.key === "ArrowRight" || e.key === ">" || e.key === ".") {
			e.preventDefault();
			setLevel((prev) => Math.min(maxLevel, prev + 1));
		}
	};

	const handleSliderChange = (e: React.ChangeEvent<HTMLInputElement>) => {
		setLevel(Number(e.target.value));
	};

	/**
	 * Handler pre Play button
	 * Zobrazí modal pre zadanie mena ak nie je nastavené
	 */
	const handlePlayClick = () => {
		// Ak už máme meno, vytvor hru rovno
		if (playerName) {
			void createGame(playerName);
		} else {
			// Inak zobraz modal
			setShowNameModal(true);
		}
	};

	/**
	 * Handler pre submit mena z modalu
	 */
	const handleNameSubmit = (name: string) => {
		setPlayerName(name);
		setShowNameModal(false);
		void createGame(name);
	};

	/**
	 * Handler pre skip (Anonymous)
	 */
	const handleNameSkip = () => {
		setPlayerName("Anonymous");
		setShowNameModal(false);
		void createGame("Anonymous");
	};

	/**
	 * Vytvorenie hry v databáze
	 */
	const createGame = async (name: string) => {
		setIsCreatingGame(true);
		setError(null);

			try {
			// Try to get gameType from localStorage as fallback
			const storedGameType = typeof window !== 'undefined' 
				? localStorage.getItem('snake_gameType') as SnakeGameType | null
				: null;
			const finalGameType = storedGameType || gameType;
			
			console.log('[Level] Creating game with gameType from context:', gameType, 'localStorage:', storedGameType, 'using:', finalGameType, 'level:', level);
			// Vytvoríme novú hru v databáze
			const response = await fetch("/api/snake/game", {
				method: "POST",
				headers: { "Content-Type": "application/json" },
				body: JSON.stringify({
					playerName: name,
					gameType: finalGameType,
					level,
				}),
			});			const result = await response.json();

			if (result.success) {
				// Uložíme gameId do contextu
				setCurrentGameId(result.data.id);
				console.log(`Game created with ID: ${result.data.id}`);
				
				// Presmerujeme na gameplay
				router.push("/games/snake/gameplay");
			} else {
				setError(result.error || "Failed to create game");
			}
		} catch (err) {
			console.error("Error creating game:", err);
			setError("Failed to create game. Please try again.");
		} finally {
			setIsCreatingGame(false);
		}
	};

	// Calculate slider fill percentage
	const fillPercentage = ((level - minLevel) / (maxLevel - minLevel)) * 100;

	return (
		<div
			className="flex min-h-screen flex-col items-center justify-start bg-black text-white pt-16"
			onKeyDown={handleKeyDown}
			tabIndex={0}
			autoFocus
		>
			{/* Back Button */}
			<Link
				href="/games/snake"
				className="absolute top-8 left-8 px-6 py-3 bg-gradient-to-b from-[#fef3c7] to-[#fde047] text-black font-['Press_Start_2P'] text-sm rounded-lg hover:from-[#fde047] to-[#facc15] transition-all"
			>
				Back
			</Link>

			{/* Title */}
			<h1 className="mb-20 font-['Press_Start_2P'] text-6xl snake-gradient-text drop-shadow-[0_0_10px_rgba(250,204,21,0.5)]">
				Level
			</h1>

			{/* Slider Container */}
			<div className="flex items-center gap-8 mb-12">
				{/* Minus Button */}
				<button
					onClick={() => setLevel((prev) => Math.max(minLevel, prev - 1))}
					className="font-['Press_Start_2P'] text-5xl snake-gradient-text hover:scale-110 transition-transform"
				>
					-
				</button>

				{/* Slider Track */}
				<div className="relative w-[400px] h-16">
					{/* Custom styled slider */}
					<div className="absolute inset-0 bg-gradient-to-r from-gray-700 via-gray-600 to-gray-700 rounded-full border-4 border-yellow-600 shadow-lg overflow-hidden">
						{/* Fill */}
						<div
							className="absolute left-0 top-0 h-full bg-gradient-to-r from-yellow-300 to-yellow-400 transition-all duration-200"
							style={{ width: `${fillPercentage}%` }}
						></div>
					</div>
					
					{/* Hidden input for accessibility and mouse interaction */}
					<input
						type="range"
						min={minLevel}
						max={maxLevel}
						value={level}
						onChange={handleSliderChange}
						className="absolute inset-0 w-full h-full opacity-0 cursor-pointer z-10"
					/>

					{/* Slider Thumb */}
					<div
						className="absolute top-1/2 -translate-y-1/2 w-12 h-12 bg-gradient-to-b from-gray-300 to-gray-500 rounded-full border-4 border-gray-700 shadow-xl transition-all duration-200 pointer-events-none z-20"
						style={{ left: `calc(${fillPercentage}% - 24px)` }}
					></div>
				</div>

				{/* Plus Button */}
				<button
					onClick={() => setLevel((prev) => Math.min(maxLevel, prev + 1))}
					className="font-['Press_Start_2P'] text-5xl snake-gradient-text hover:scale-110 transition-transform"
				>
					+
				</button>
			</div>

			{/* Level Display */}
			<div className="mb-16 font-['Press_Start_2P'] text-6xl snake-gradient-text drop-shadow-[0_0_8px_rgba(250,204,21,0.6)]">
				{level}
			</div>

			{/* Error Message */}
			{error && (
				<div className="mb-4 px-4 py-2 bg-red-900 border-2 border-red-500 rounded text-red-200 font-['Press_Start_2P'] text-xs">
					{error}
				</div>
			)}

			{/* Play Button */}
			<button
				onClick={handlePlayClick}
				disabled={isCreatingGame}
				className="px-12 py-4 bg-gradient-to-b from-[#fef3c7] to-[#fde047] text-black font-['Press_Start_2P'] text-2xl rounded-xl border-4 border-black hover:from-[#fde047] hover:to-[#facc15] transition-all shadow-lg hover:scale-105 disabled:opacity-50 disabled:cursor-not-allowed"
			>
				{isCreatingGame ? "Creating..." : "Play"}
			</button>

			{/* Controls hint */}
			<div className="absolute bottom-8 text-center">
				<p className="font-['Press_Start_2P'] text-sm text-gray-400">
					Use ←→ or drag slider • Click +/- buttons
				</p>
			</div>

			{/* Player Name Modal */}
			<PlayerNameModal
				isOpen={showNameModal}
				onSubmit={handleNameSubmit}
				onSkip={handleNameSkip}
			/>
		</div>
	);
}
