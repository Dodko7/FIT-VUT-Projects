/**
 * Mobile Touch Controls Component
 * 
 * Virtuálny D-pad pre mobilné zariadenia
 * Poskytuje tlačidlá pre ovládanie hry na dotykovej obrazovke
 * 
 * @author Jozef Ondrejicka
 */

"use client";

import { useState } from "react";

interface TouchControlsProps {
	onDirectionChange: (direction: "UP" | "DOWN" | "LEFT" | "RIGHT") => void;
	onPause: () => void;
	disabled?: boolean;
}

export default function TouchControls({
	onDirectionChange,
	onPause,
	disabled = false,
}: TouchControlsProps) {
	const [activeButton, setActiveButton] = useState<string | null>(null);

	const handleDirectionPress = (direction: "UP" | "DOWN" | "LEFT" | "RIGHT") => {
		if (disabled) return;
		setActiveButton(direction);
		onDirectionChange(direction);
	};

	const handleDirectionRelease = () => {
		setActiveButton(null);
	};

	const buttonBaseClass = "w-16 h-16 rounded-lg font-['Press_Start_2P'] text-2xl flex items-center justify-center transition-all select-none";
	const buttonNormalClass = "bg-gradient-to-b from-gray-700 to-gray-900 text-white border-2 border-gray-600";
	const buttonActiveClass = "bg-gradient-to-b from-yellow-500 to-yellow-700 text-black border-2 border-yellow-400 scale-95";
	const buttonDisabledClass = "bg-gray-800 text-gray-600 border-2 border-gray-700 cursor-not-allowed";

	const getButtonClass = (direction: string) => {
		if (disabled) return `${buttonBaseClass} ${buttonDisabledClass}`;
		return `${buttonBaseClass} ${activeButton === direction ? buttonActiveClass : buttonNormalClass}`;
	};

	return (
		<div className="flex flex-col items-center gap-4 p-4 bg-black bg-opacity-50 rounded-xl border-2 border-gray-700">
			{/* D-pad Grid */}
			<div className="grid grid-cols-3 gap-2">
				{/* Top Row */}
				<div></div>
				<button
					onTouchStart={() => handleDirectionPress("UP")}
					onTouchEnd={handleDirectionRelease}
					onMouseDown={() => handleDirectionPress("UP")}
					onMouseUp={handleDirectionRelease}
					onMouseLeave={handleDirectionRelease}
					disabled={disabled}
					className={getButtonClass("UP")}
				>
					↑
				</button>
				<div></div>

				{/* Middle Row */}
				<button
					onTouchStart={() => handleDirectionPress("LEFT")}
					onTouchEnd={handleDirectionRelease}
					onMouseDown={() => handleDirectionPress("LEFT")}
					onMouseUp={handleDirectionRelease}
					onMouseLeave={handleDirectionRelease}
					disabled={disabled}
					className={getButtonClass("LEFT")}
				>
					←
				</button>
				<div className="w-16 h-16"></div>
				<button
					onTouchStart={() => handleDirectionPress("RIGHT")}
					onTouchEnd={handleDirectionRelease}
					onMouseDown={() => handleDirectionPress("RIGHT")}
					onMouseUp={handleDirectionRelease}
					onMouseLeave={handleDirectionRelease}
					disabled={disabled}
					className={getButtonClass("RIGHT")}
				>
					→
				</button>

				{/* Bottom Row */}
				<div></div>
				<button
					onTouchStart={() => handleDirectionPress("DOWN")}
					onTouchEnd={handleDirectionRelease}
					onMouseDown={() => handleDirectionPress("DOWN")}
					onMouseUp={handleDirectionRelease}
					onMouseLeave={handleDirectionRelease}
					disabled={disabled}
					className={getButtonClass("DOWN")}
				>
					↓
				</button>
				<div></div>
			</div>

			{/* Pause Button */}
			<button
				onClick={onPause}
				disabled={disabled}
				className={`w-full px-6 py-3 rounded-lg font-['Press_Start_2P'] text-sm transition-all ${
					disabled
						? "bg-gray-800 text-gray-600 border-2 border-gray-700 cursor-not-allowed"
						: "bg-gradient-to-b from-purple-500 to-purple-700 text-white border-2 border-purple-400 hover:from-purple-600 hover:to-purple-800"
				}`}
			>
				⏸ Pause
			</button>
		</div>
	);
}
