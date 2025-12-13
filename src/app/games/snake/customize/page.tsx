/**
 * Snake Customize Page
 * 
 * @author Igor Lacko
 * @description Customization settings for Snake game appearance
 */

"use client";

import { useState, useEffect } from "react";
import Link from "next/link";
import { useRouter } from "next/navigation";

// Color presets for different elements
const SNAKE_COLORS = [
	{ name: "Classic Green", head: "#10b981", body: "#34d399" },
	{ name: "Blue", head: "#3b82f6", body: "#60a5fa" },
	{ name: "Purple", head: "#a855f7", body: "#c084fc" },
	{ name: "Red", head: "#ef4444", body: "#f87171" },
	{ name: "Orange", head: "#f97316", body: "#fb923c" },
	{ name: "Pink", head: "#ec4899", body: "#f472b6" },
	{ name: "Yellow", head: "#eab308", body: "#fde047" },
	{ name: "Cyan", head: "#06b6d4", body: "#22d3ee" },
];

const FOOD_COLORS = [
	{ name: "Red Apple", color: "#ef4444" },
	{ name: "Green Apple", color: "#22c55e" },
	{ name: "Golden Apple", color: "#fbbf24" },
	{ name: "Purple Berry", color: "#a855f7" },
	{ name: "Orange", color: "#f97316" },
	{ name: "Pink", color: "#ec4899" },
];

const BACKGROUND_COLORS = [
	{ name: "Dark", color: "#000000" },
	{ name: "Navy", color: "#1e3a8a" },
	{ name: "Forest", color: "#14532d" },
	{ name: "Charcoal", color: "#1f2937" },
	{ name: "Purple Dark", color: "#581c87" },
	{ name: "Brown", color: "#431407" },
];

const WALL_COLORS = [
	{ name: "Golden", color: "#fbbf24" },
	{ name: "Red", color: "#dc2626" },
	{ name: "Blue", color: "#2563eb" },
	{ name: "Green", color: "#16a34a" },
	{ name: "Purple", color: "#9333ea" },
	{ name: "White", color: "#f3f4f6" },
];

const GRID_STYLES = [
	{ name: "None", value: "none" },
	{ name: "Subtle", value: "subtle" },
	{ name: "Visible", value: "visible" },
];

export default function SnakeCustomizePage() {
	const router = useRouter();
	
	// Load settings from localStorage
	const [snakeColor, setSnakeColor] = useState(0);
	const [foodColor, setFoodColor] = useState(0);
	const [backgroundColor, setBackgroundColor] = useState(0);
	const [wallColor, setWallColor] = useState(0);
	const [gridStyle, setGridStyle] = useState(0);

	// Load saved settings on mount
	useEffect(() => {
		const savedSettings = localStorage.getItem("snake_customization");
		if (savedSettings) {
			try {
				const settings = JSON.parse(savedSettings);
				setSnakeColor(settings.snakeColor ?? 0);
				setFoodColor(settings.foodColor ?? 0);
				setBackgroundColor(settings.backgroundColor ?? 0);
				setWallColor(settings.wallColor ?? 0);
				setGridStyle(settings.gridStyle ?? 0);
			} catch (error) {
				console.error("Failed to load customization settings:", error);
			}
		}
	}, []);

	// Save settings
	const handleSave = () => {
		const settings = {
			snakeColor,
			foodColor,
			backgroundColor,
			wallColor,
			gridStyle,
			snakeColorData: SNAKE_COLORS[snakeColor],
			foodColorData: FOOD_COLORS[foodColor],
			backgroundColorData: BACKGROUND_COLORS[backgroundColor],
			wallColorData: WALL_COLORS[wallColor],
			gridStyleData: GRID_STYLES[gridStyle],
		};
		
		localStorage.setItem("snake_customization", JSON.stringify(settings));
		router.push("/games/snake");
	};

	// Reset to defaults
	const handleReset = () => {
		setSnakeColor(0);
		setFoodColor(0);
		setBackgroundColor(0);
		setWallColor(0);
		setGridStyle(0);
	};

	// Keyboard navigation
	const handleKeyDown = (e: React.KeyboardEvent) => {
		if (e.key === "Escape") {
			router.push("/games/snake");
		}
	};

	return (
		<div
			className="flex min-h-screen flex-col items-center justify-start bg-black text-white pt-8 pb-16"
			onKeyDown={handleKeyDown}
			tabIndex={0}
			autoFocus
		>
			{/* Back Button */}
			<Link
				href="/games/snake"
				className="absolute top-8 left-8 px-6 py-3 bg-gradient-to-b from-[#fef3c7] to-[#fde047] text-black font-['Press_Start_2P'] text-sm rounded-lg hover:from-[#fde047] hover:to-[#facc15] transition-all"
			>
				Back
			</Link>

			{/* Title */}
			<h1 className="mb-8 font-['Press_Start_2P'] text-5xl snake-gradient-text drop-shadow-[0_0_10px_rgba(250,204,21,0.5)]">
				Customize
			</h1>

			{/* Preview */}
			<div className="mb-8 p-4 bg-gray-900 border-4 border-yellow-600 rounded-lg">
				<h2 className="font-['Press_Start_2P'] text-sm text-yellow-400 mb-4 text-center">
					Preview
				</h2>
				<div
					className="w-80 h-80 rounded-lg relative overflow-hidden mx-auto"
					style={{
						backgroundColor: BACKGROUND_COLORS[backgroundColor]?.color ?? "#000000",
					}}
				>
					{/* Grid overlay */}
					{gridStyle > 0 && (
						<div 
							className="absolute inset-0 pointer-events-none"
							style={{
								backgroundImage: `
									repeating-linear-gradient(
										0deg,
										transparent,
										transparent 19px,
										rgba(255, 255, 255, ${gridStyle === 1 ? "0.15" : "0.5"}) 19px,
										rgba(255, 255, 255, ${gridStyle === 1 ? "0.15" : "0.5"}) 20px
									),
									repeating-linear-gradient(
										90deg,
										transparent,
										transparent 19px,
										rgba(255, 255, 255, ${gridStyle === 1 ? "0.15" : "0.5"}) 19px,
										rgba(255, 255, 255, ${gridStyle === 1 ? "0.15" : "0.5"}) 20px
									)
								`,
							}}
						></div>
					)}
					
					{/* Wall border (visible border like in game) */}
					<div
						className="absolute inset-0 pointer-events-none"
						style={{
							border: `4px solid ${WALL_COLORS[wallColor]?.color ?? "#fbbf24"}`,
						}}
					></div>
					
					{/* Sample snake body segments - positioned within grid cells (16 cells = 320px, each cell = 20px) */}
					<div
						className="absolute rounded-sm"
						style={{
							backgroundColor: SNAKE_COLORS[snakeColor]?.body ?? "#34d399",
							width: "19px",
							height: "19px",
							top: "140.5px", // row 7 (0-indexed): 7 * 20px + 0.5px
							left: "80.5px", // col 4: 4 * 20px + 0.5px
							boxShadow: `0 0 8px ${SNAKE_COLORS[snakeColor]?.body ?? "#34d399"}`,
						}}
					></div>
					<div
						className="absolute rounded-sm"
						style={{
							backgroundColor: SNAKE_COLORS[snakeColor]?.body ?? "#34d399",
							width: "19px",
							height: "19px",
							top: "140.5px",
							left: "100.5px", // col 5
							boxShadow: `0 0 8px ${SNAKE_COLORS[snakeColor]?.body ?? "#34d399"}`,
						}}
					></div>
					<div
						className="absolute rounded-sm"
						style={{
							backgroundColor: SNAKE_COLORS[snakeColor]?.body ?? "#34d399",
							width: "19px",
							height: "19px",
							top: "140.5px",
							left: "120.5px", // col 6
							boxShadow: `0 0 8px ${SNAKE_COLORS[snakeColor]?.body ?? "#34d399"}`,
						}}
					></div>
					
					{/* Sample snake head */}
					<div
						className="absolute rounded-sm"
						style={{
							backgroundColor: SNAKE_COLORS[snakeColor]?.head ?? "#10b981",
							width: "19px",
							height: "19px",
							top: "140.5px",
							left: "140.5px", // col 7
							boxShadow: `0 0 12px ${SNAKE_COLORS[snakeColor]?.head ?? "#10b981"}`,
						}}
					></div>
					
					{/* Sample food (apple) */}
					<div
						className="absolute rounded-full"
						style={{
							backgroundColor: FOOD_COLORS[foodColor]?.color ?? "#ef4444",
							width: "19px",
							height: "19px",
							top: "80.5px", // row 4
							left: "200.5px", // col 10
							boxShadow: `0 0 10px ${FOOD_COLORS[foodColor]?.color ?? "#ef4444"}`,
						}}
					></div>
				</div>
			</div>

			<div className="w-full max-w-4xl px-8 space-y-8">
				{/* Snake Color */}
				<div className="bg-gray-900 border-4 border-yellow-600 rounded-lg p-6">
					<h2 className="font-['Press_Start_2P'] text-xl text-yellow-400 mb-4">
						Snake Color
					</h2>
					<div className="grid grid-cols-2 md:grid-cols-4 gap-4">
						{SNAKE_COLORS.map((color, index) => (
							<button
								key={index}
								onClick={() => setSnakeColor(index)}
								className={`p-4 rounded-lg border-4 transition-all ${
									snakeColor === index
										? "border-yellow-400 scale-105"
										: "border-gray-600 hover:border-gray-400"
								}`}
							>
								<div className="flex gap-2 mb-2">
									<div
										className="w-8 h-8 rounded"
										style={{ backgroundColor: color.head }}
									></div>
									<div
										className="w-8 h-8 rounded"
										style={{ backgroundColor: color.body }}
									></div>
								</div>
								<p className="font-['Press_Start_2P'] text-xs text-gray-300">
									{color.name}
								</p>
							</button>
						))}
					</div>
				</div>

				{/* Food Color */}
				<div className="bg-gray-900 border-4 border-yellow-600 rounded-lg p-6">
					<h2 className="font-['Press_Start_2P'] text-xl text-yellow-400 mb-4">
						Food Color
					</h2>
					<div className="grid grid-cols-2 md:grid-cols-3 gap-4">
						{FOOD_COLORS.map((color, index) => (
							<button
								key={index}
								onClick={() => setFoodColor(index)}
								className={`p-4 rounded-lg border-4 transition-all ${
									foodColor === index
										? "border-yellow-400 scale-105"
										: "border-gray-600 hover:border-gray-400"
								}`}
							>
								<div
									className="w-12 h-12 rounded-full mx-auto mb-2"
									style={{ backgroundColor: color.color }}
								></div>
								<p className="font-['Press_Start_2P'] text-xs text-gray-300">
									{color.name}
								</p>
							</button>
						))}
					</div>
				</div>

				{/* Background Color */}
				<div className="bg-gray-900 border-4 border-yellow-600 rounded-lg p-6">
					<h2 className="font-['Press_Start_2P'] text-xl text-yellow-400 mb-4">
						Background
					</h2>
					<div className="grid grid-cols-2 md:grid-cols-3 gap-4">
						{BACKGROUND_COLORS.map((color, index) => (
							<button
								key={index}
								onClick={() => setBackgroundColor(index)}
								className={`p-4 rounded-lg border-4 transition-all ${
									backgroundColor === index
										? "border-yellow-400 scale-105"
										: "border-gray-600 hover:border-gray-400"
								}`}
							>
								<div
									className="w-full h-16 rounded mb-2"
									style={{ backgroundColor: color.color }}
								></div>
								<p className="font-['Press_Start_2P'] text-xs text-gray-300">
									{color.name}
								</p>
							</button>
						))}
					</div>
				</div>

				{/* Wall Color */}
				<div className="bg-gray-900 border-4 border-yellow-600 rounded-lg p-6">
					<h2 className="font-['Press_Start_2P'] text-xl text-yellow-400 mb-4">
						Wall / Border Color
					</h2>
					<div className="grid grid-cols-2 md:grid-cols-3 gap-4">
						{WALL_COLORS.map((color, index) => (
							<button
								key={index}
								onClick={() => setWallColor(index)}
								className={`p-4 rounded-lg border-4 transition-all ${
									wallColor === index
										? "border-yellow-400 scale-105"
										: "border-gray-600 hover:border-gray-400"
								}`}
							>
								<div
									className="w-full h-16 rounded border-8"
									style={{ borderColor: color.color, backgroundColor: "#000" }}
								></div>
								<p className="font-['Press_Start_2P'] text-xs text-gray-300 mt-2">
									{color.name}
								</p>
							</button>
						))}
					</div>
				</div>

				{/* Grid Style */}
				<div className="bg-gray-900 border-4 border-yellow-600 rounded-lg p-6">
					<h2 className="font-['Press_Start_2P'] text-xl text-yellow-400 mb-4">
						Grid Lines
					</h2>
					<div className="grid grid-cols-3 gap-4">
						{GRID_STYLES.map((style, index) => (
							<button
								key={index}
								onClick={() => setGridStyle(index)}
								className={`p-4 rounded-lg border-4 transition-all ${
									gridStyle === index
										? "border-yellow-400 scale-105"
										: "border-gray-600 hover:border-gray-400"
								}`}
							>
								<p className="font-['Press_Start_2P'] text-sm text-gray-300">
									{style.name}
								</p>
							</button>
						))}
					</div>
				</div>

				{/* Action Buttons */}
				<div className="flex gap-4 justify-center mt-8">
					<button
						onClick={handleReset}
						className="px-8 py-4 bg-gradient-to-b from-gray-600 to-gray-800 text-white font-['Press_Start_2P'] text-sm rounded-lg hover:from-gray-700 hover:to-gray-900 transition-all"
					>
						Reset
					</button>
					<button
						onClick={handleSave}
						className="px-8 py-4 bg-gradient-to-b from-yellow-400 to-yellow-600 text-black font-['Press_Start_2P'] text-sm rounded-lg hover:from-yellow-500 hover:to-yellow-700 transition-all"
					>
						Save
					</button>
				</div>
			</div>
		</div>
	);
}
