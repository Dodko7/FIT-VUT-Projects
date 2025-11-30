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

	/**
	 * Handler pre výber game type
	 */
	const handleSelect = (type: SnakeGameType) => {
		setGameType(type);
		console.log(`Game type changed to: ${type}`);
		router.push("/games/snake");
	};

	const handleKeyDown = (e: React.KeyboardEvent) => {
		if (e.key === "ArrowUp" || e.key === "w" || e.key === "W") {
			e.preventDefault();
			setSelectedIndex((prev) => (prev > 0 ? prev - 1 : gameTypes.length - 1));
		} else if (e.key === "ArrowDown" || e.key === "s" || e.key === "S") {
			e.preventDefault();
			setSelectedIndex((prev) => (prev < gameTypes.length - 1 ? prev + 1 : 0));
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
						onMouseEnter={() => setSelectedIndex(index)}
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

			{/* Controls hint */}
			<div className="absolute bottom-8 text-center">
				<p className="font-['Press_Start_2P'] text-sm text-gray-400">
					Use ↑↓ or W/S to navigate • Enter/Space to select
				</p>
			</div>
		</div>
	);
}
