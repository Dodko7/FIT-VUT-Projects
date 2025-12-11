/**
 * Snake Game Type Selection Page
 * 
 * @author Igor Lacko
 * @description Výber typu hry (CLASSIC, BOX, CAMPAIGN)
 *              Ukladá výber do Context a vracia na menu
 */

"use client";

import { useState } from "react";
import Link from "next/link";
import { useRouter } from "next/navigation";
import { useSnakeGame } from "~/contexts/SnakeGameContext";
import type { SnakeGameType } from "~/lib/types/snake";

const GAME_TYPE_LABELS: Record<SnakeGameType, string> = {
	CLASSIC: "Classic",
	BOX: "Box",
	CAMPAIGN: "Campaign",
};

export default function SnakeGameTypePage() {
	const { gameType, setGameType } = useSnakeGame();
	const router = useRouter();
	
	const gameTypes: SnakeGameType[] = ["CLASSIC", "BOX", "CAMPAIGN"];
	
	// Inicializácia selectedIndex podľa aktuálneho gameType z contextu
	const initialIndex = gameTypes.indexOf(gameType);
	const [selectedIndex, setSelectedIndex] = useState(
		initialIndex !== -1 ? initialIndex : 0,
	);
	
	// Sync selectedIndex with arrow key selection in real-time
	const handleIndexChange = (newIndex: number) => {
		setSelectedIndex(newIndex);
		const selectedType = gameTypes[newIndex];
		if (selectedType) {
			console.log('[GameType] Arrow key selected:', selectedType);
			setGameType(selectedType);
			// Also save to localStorage
			if (typeof window !== 'undefined') {
				localStorage.setItem('snake_gameType', selectedType);
			}
		}
	};

	/**
	 * Handler pre výber game type
	 */
	const handleSelect = (type: SnakeGameType) => {
		console.log('[GameType] Setting gameType in context to:', type);
		setGameType(type);
		// Also save to localStorage as backup
		if (typeof window !== 'undefined') {
			localStorage.setItem('snake_gameType', type);
		}
		console.log(`[GameType] Game type changed to: ${type}`);
		router.push("/games/snake");
	};

	const handleKeyDown = (e: React.KeyboardEvent) => {
		if (e.key === "ArrowUp" || e.key === "w" || e.key === "W") {
			e.preventDefault();
			const newIndex = selectedIndex > 0 ? selectedIndex - 1 : gameTypes.length - 1;
			handleIndexChange(newIndex);
		} else if (e.key === "ArrowDown" || e.key === "s" || e.key === "S") {
			e.preventDefault();
			const newIndex = selectedIndex < gameTypes.length - 1 ? selectedIndex + 1 : 0;
			handleIndexChange(newIndex);
		} else if (e.key === "Enter" || e.key === " ") {
			e.preventDefault();
			handleSelect(gameTypes[selectedIndex]!);
		}
	};

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
				GameType
			</h1>

			{/* Game Type Options */}
			<div className="flex flex-col gap-8 items-center">
				{gameTypes.map((type, index: number) => (
					<button
						key={type}
						onClick={() => handleSelect(type)}
						onMouseEnter={() => handleIndexChange(index)}
						className="group flex items-center justify-center gap-4 transition-all duration-200 min-w-[400px]"
					>
						{/* Arrow indicator */}
						<span
							className={`font-['Press_Start_2P'] text-3xl transition-opacity ${
								selectedIndex === index ? "opacity-100 snake-gradient-text" : "opacity-0"
							}`}
						>
							&gt;
						</span>

						{/* Game type text */}
						<span
							className={`font-['Press_Start_2P'] text-3xl transition-colors ${
								selectedIndex === index
									? "snake-gradient-text drop-shadow-[0_0_8px_rgba(250,204,21,0.6)]"
									: "text-yellow-200"
							}`}
						>
							{GAME_TYPE_LABELS[type]}
						</span>
					</button>
				))}
			</div>

			{/* Preview of playing area for selected game type */}
			<div className="mt-12 p-4 bg-gray-900 rounded-lg border-2 border-yellow-500/50 flex flex-col items-center">
				<canvas
					ref={(canvas) => {
						if (canvas) {
							const ctx = canvas.getContext("2d");
							if (ctx) {
								canvas.width = 200;
								canvas.height = 200;
								
								const selectedType = gameTypes[selectedIndex];
								
								// Clear canvas
								ctx.fillStyle = "#000000";
								ctx.fillRect(0, 0, 200, 200);
								
								const cellSize = 10;
								const gridSize = 20;
								
								// Draw grid
								ctx.strokeStyle = "#1a1a1a";
								for (let i = 0; i <= gridSize; i++) {
									ctx.beginPath();
									ctx.moveTo(i * cellSize, 0);
									ctx.lineTo(i * cellSize, 200);
									ctx.stroke();
									ctx.beginPath();
									ctx.moveTo(0, i * cellSize);
									ctx.lineTo(200, i * cellSize);
									ctx.stroke();
								}
								
								// Draw border around entire playground
								ctx.strokeStyle = "#facc15";
								ctx.lineWidth = 3;
								ctx.strokeRect(0, 0, 200, 200);
								ctx.lineWidth = 1;
								
								// Draw walls for BOX mode
								if (selectedType === "BOX") {
									ctx.fillStyle = "#facc15"; // Yellow color matching the text
									for (let i = 0; i < gridSize; i++) {
										ctx.fillRect(0, i * cellSize, cellSize, cellSize);
										ctx.fillRect((gridSize - 1) * cellSize, i * cellSize, cellSize, cellSize);
										ctx.fillRect(i * cellSize, 0, cellSize, cellSize);
										ctx.fillRect(i * cellSize, (gridSize - 1) * cellSize, cellSize, cellSize);
									}
								}
								
								// Draw "+" barrier for CAMPAIGN mode (smaller by 2 on each end)
								if (selectedType === "CAMPAIGN") {
									ctx.fillStyle = "#facc15"; // Yellow color matching the text
									const mid = Math.floor(gridSize / 2);
									
									// Vertical line of the "+" (from 6 to 14 - extended bottom by 1)
									for (let i = 6; i < gridSize - 5; i++) {
										ctx.fillRect(mid * cellSize, i * cellSize, cellSize, cellSize);
									}
									
									// Horizontal line of the "+" (from 6 to 14 - extended right by 1)
									for (let i = 6; i < gridSize - 5; i++) {
										ctx.fillRect(i * cellSize, mid * cellSize, cellSize, cellSize);
									}
								}
								
								// Draw sample snake (adjust position for CAMPAIGN mode)
								ctx.fillStyle = "#00ff00";
								const snakeY = selectedType === "CAMPAIGN" ? 12 : 10; // 2 blocks down for CAMPAIGN
								ctx.fillRect(5 * cellSize, snakeY * cellSize, cellSize, cellSize);
								ctx.fillRect(6 * cellSize, snakeY * cellSize, cellSize, cellSize);
								ctx.fillRect(7 * cellSize, snakeY * cellSize, cellSize, cellSize);
								
								// Draw sample food (adjust position for CAMPAIGN mode)
								ctx.fillStyle = "#ff0000";
								const foodY = selectedType === "CAMPAIGN" ? 8 : 10; // 2 blocks up for CAMPAIGN
								ctx.fillRect(14 * cellSize, foodY * cellSize, cellSize, cellSize);
							}
						}
					}}
				/>
				<p className="text-center text-xs font-['Press_Start_2P'] text-gray-400 mt-2">
					{gameTypes[selectedIndex] === "CLASSIC" && "No walls - wrap around edges"}
					{gameTypes[selectedIndex] === "BOX" && "Walls on all sides"}
					{gameTypes[selectedIndex] === "CAMPAIGN" && "Cross barrier in the middle"}
				</p>
			</div>

			{/* Controls hint */}
			<div className="absolute bottom-8 text-center">
				<p className="font-['Press_Start_2P'] text-sm text-gray-400">
					Use ↑↓ or W/S to navigate • Enter/Space to select
				</p>
			</div>
		</div>
	);
}
