/**
 * Snake Main Menu Page
 * 
 * @author Igor Lacko
 * @description Hlavné menu Snake hry s navigáciou a zobrazením aktuálnych nastavení
 */

"use client";

import { useState } from "react";
import Link from "next/link";
import { useSnakeGame } from "~/contexts/SnakeGameContext";

type MenuOption = {
	label: string;
	action: () => void;
};

const GAME_TYPE_LABELS = {
	CLASSIC: "Classic",
	BOX: "Box",
	CAMPAIGN: "Campaign",
} as const;

export default function SnakeMenuPage() {
	const { gameType, level } = useSnakeGame();
	const [selectedIndex, setSelectedIndex] = useState(0);
	
	console.log('[Menu] Current gameType from context:', gameType, 'level:', level);

	const menuOptions: MenuOption[] = [
		{
			label: "Play",
			action: () => {
				window.location.href = "/games/snake/level";
			},
		},
		{
			label: "Gametype",
			action: () => {
				window.location.href = "/games/snake/gametype";
			},
		},
		{
			label: "Customise",
			action: () => {
				window.location.href = "/games/snake/customize";
			},
		},
		{
			label: "Leaderboard",
			action: () => {
				window.location.href = "/games/snake/leaderboard";
			},
		},
		{
			label: "Controls",
			action: () => {
				window.location.href = "/games/snake/controls";
			},
		},
		{
			label: "Exit",
			action: () => {
				window.location.href = "/";
			},
		},
	];

	const handleKeyDown = (e: React.KeyboardEvent) => {
		if (e.key === "ArrowUp" || e.key === "w" || e.key === "W") {
			e.preventDefault();
			setSelectedIndex((prev) => (prev > 0 ? prev - 1 : menuOptions.length - 1));
		} else if (e.key === "ArrowDown" || e.key === "s" || e.key === "S") {
			e.preventDefault();
			setSelectedIndex((prev) => (prev < menuOptions.length - 1 ? prev + 1 : 0));
		} else if (e.key === "Enter" || e.key === " ") {
			e.preventDefault();
			menuOptions[selectedIndex]?.action();
		}
	};

	return (
		<div
			className="flex min-h-screen flex-col items-center justify-center bg-black text-white"
			onKeyDown={handleKeyDown}
			tabIndex={0}
			autoFocus
		>
			{/* Title */}
			<h1 className="mb-16 font-['Press_Start_2P'] text-6xl snake-gradient-text drop-shadow-[0_0_10px_rgba(250,204,21,0.5)] self-center">
				Snake
			</h1>

			{/* Menu Options */}
			<div className="flex flex-col gap-6 items-center">
				{menuOptions.map((option, index) => (
					<button
						key={option.label}
						onClick={() => {
							setSelectedIndex(index);
							option.action();
						}}
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

						{/* Menu text */}
						<span
							className={`font-['Press_Start_2P'] text-3xl transition-colors ${
								selectedIndex === index
									? "snake-gradient-text drop-shadow-[0_0_8px_rgba(250,204,21,0.6)]"
									: "text-yellow-200"
							}`}
						>
							{option.label}
						</span>
					</button>
				))}
			</div>

			{/* Current Settings Display */}
			<div className="absolute top-8 right-8 bg-gray-900 bg-opacity-80 border-2 border-yellow-600 rounded-lg p-4 font-['Press_Start_2P'] text-xs">
				<p className="text-yellow-400 mb-2">Current Settings:</p>
				<p className="text-yellow-200">Mode: {GAME_TYPE_LABELS[gameType]}</p>
				<p className="text-yellow-200">Level: {level}</p>
			</div>

			{/* Controls hint */}
			<div className="absolute bottom-8 text-center">
				<p className="font-['Press_Start_2P'] text-sm text-gray-400">
					Use ↑↓ or W/S to navigate • Enter/Space to select
				</p>
			</div>

			{/* Back button */}
			<Link
				href="/"
				className="absolute top-8 left-8 font-['Press_Start_2P'] text-sm text-yellow-400 hover:text-yellow-200 transition-colors"
			>
				← Back to Arcade
			</Link>
		</div>
	);
}
