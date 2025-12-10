"use client";

import Link from "next/link";
import { useState, useEffect } from "react";

type PressedKeys = {
	up: boolean;
	down: boolean;
	left: boolean;
	right: boolean;
	esc: boolean;
};

export default function SnakeControlsPage() {
	const [pressedKeys, setPressedKeys] = useState<PressedKeys>({
		up: false,
		down: false,
		left: false,
		right: false,
		esc: false,
	});

	useEffect(() => {
		const handleKeyDown = (e: KeyboardEvent) => {
			const key = e.key.toLowerCase();
			setPressedKeys((prev) => {
				const updated = { ...prev };
				if (key === "arrowup" || key === "w") updated.up = true;
				if (key === "arrowdown" || key === "s") updated.down = true;
				if (key === "arrowleft" || key === "a") updated.left = true;
				if (key === "arrowright" || key === "d") updated.right = true;
				if (key === "escape") updated.esc = true;
				return updated;
			});
		};

		const handleKeyUp = (e: KeyboardEvent) => {
			const key = e.key.toLowerCase();
			setPressedKeys((prev) => {
				const updated = { ...prev };
				if (key === "arrowup" || key === "w") updated.up = false;
				if (key === "arrowdown" || key === "s") updated.down = false;
				if (key === "arrowleft" || key === "a") updated.left = false;
				if (key === "arrowright" || key === "d") updated.right = false;
				if (key === "escape") updated.esc = false;
				return updated;
			});
		};

		window.addEventListener("keydown", handleKeyDown);
		window.addEventListener("keyup", handleKeyUp);

		return () => {
			window.removeEventListener("keydown", handleKeyDown);
			window.removeEventListener("keyup", handleKeyUp);
		};
	}, []);

	// Funkcia pre získanie tried tlačidla
	const getButtonClass = (isPressed: boolean) => {
		return isPressed
			? "w-24 h-24 bg-gradient-to-b from-[#10b981] via-[#059669] to-[#047857] rounded-2xl border-4 border-[#065f46] shadow-[0_0_20px_rgba(16,185,129,0.8)] flex items-center justify-center transition-all duration-100 scale-95"
			: "w-24 h-24 bg-gradient-to-b from-[#fef3c7] via-[#fde047] to-[#fde047] rounded-2xl border-4 border-black shadow-lg flex items-center justify-center transition-all duration-100";
	};

	const getEscButtonClass = (isPressed: boolean) => {
		return isPressed
			? "w-32 h-24 bg-gradient-to-b from-[#10b981] via-[#059669] to-[#047857] rounded-2xl border-4 border-[#065f46] shadow-[0_0_20px_rgba(16,185,129,0.8)] flex items-center justify-center transition-all duration-100 scale-95"
			: "w-32 h-24 bg-gradient-to-b from-[#fef3c7] via-[#fde047] to-[#fde047] rounded-2xl border-4 border-black shadow-lg flex items-center justify-center transition-all duration-100";
	};

	const getArrowColor = (isPressed: boolean) => (isPressed ? "white" : "black");
	const getTextColor = (isPressed: boolean) => (isPressed ? "white" : "black");

	return (
		<div className="flex min-h-screen flex-col items-center justify-start bg-black text-white pt-16">
			{/* Back Button */}
			<Link
				href="/games/snake"
				className="absolute top-8 left-8 px-6 py-3 bg-gradient-to-b from-[#fef3c7] to-[#fde047] text-black font-['Press_Start_2P'] text-sm rounded-lg hover:from-[#fde047] to-[#facc15] transition-all"
			>
				Back
			</Link>

			{/* Title */}
			<h1 className="mb-20 font-['Press_Start_2P'] text-6xl snake-gradient-text drop-shadow-[0_0_10px_rgba(250,204,21,0.5)]">
				Controls
			</h1>

			{/* Controls Display */}
			<div className="flex flex-col items-center gap-16">
				{/* Up Arrow */}
				<div className="flex items-center justify-center">
					<div className={getButtonClass(pressedKeys.up)}>
						<div
							className="w-0 h-0 border-l-[20px] border-l-transparent border-r-[20px] border-r-transparent border-b-[30px]"
							style={{ borderBottomColor: getArrowColor(pressedKeys.up) }}
						></div>
						<span
							className={`absolute font-['Press_Start_2P'] text-xl mt-1 text-${getTextColor(pressedKeys.up)}`}
							style={{ color: getTextColor(pressedKeys.up) }}
						>
							w
						</span>
					</div>
				</div>

				{/* Left, Down, Right Arrows */}
				<div className="flex items-center gap-6">
					{/* Left Arrow */}
					<div className={getButtonClass(pressedKeys.left)}>
						<div
							className="w-0 h-0 border-t-[20px] border-t-transparent border-b-[20px] border-b-transparent border-r-[30px]"
							style={{ borderRightColor: getArrowColor(pressedKeys.left) }}
						></div>
						<span
							className={`absolute font-['Press_Start_2P'] text-xl ml-1`}
							style={{ color: getTextColor(pressedKeys.left) }}
						>
							a
						</span>
					</div>

					{/* Down Arrow */}
					<div className={getButtonClass(pressedKeys.down)}>
						<div
							className="w-0 h-0 border-l-[20px] border-l-transparent border-r-[20px] border-r-transparent border-t-[30px]"
							style={{ borderTopColor: getArrowColor(pressedKeys.down) }}
						></div>
						<span
							className={`absolute font-['Press_Start_2P'] text-xl mb-1`}
							style={{ color: getTextColor(pressedKeys.down) }}
						>
							s
						</span>
					</div>

					{/* Right Arrow */}
					<div className={getButtonClass(pressedKeys.right)}>
						<div
							className="w-0 h-0 border-t-[20px] border-t-transparent border-b-[20px] border-b-transparent border-l-[30px]"
							style={{ borderLeftColor: getArrowColor(pressedKeys.right) }}
						></div>
						<span
							className={`absolute font-['Press_Start_2P'] text-xl mr-1`}
							style={{ color: getTextColor(pressedKeys.right) }}
						>
							d
						</span>
					</div>
				</div>

				{/* ESC Key */}
				<div className="flex items-center gap-8 mt-8">
					<div className={getEscButtonClass(pressedKeys.esc)}>
						<span
							className={`font-['Press_Start_2P'] text-xl`}
							style={{ color: getTextColor(pressedKeys.esc) }}
						>
							esc
						</span>
					</div>
					<span className="font-['Press_Start_2P'] text-3xl snake-gradient-text">
						Pause/Play
					</span>
				</div>
			</div>
		</div>
	);
}
