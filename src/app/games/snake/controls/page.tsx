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

	const getArrowColor = (isPressed: boolean) => (isPressed ? "#ffffff" : "#000000");
	const getTextColor = (isPressed: boolean) => (isPressed ? "#000000" : "#ffffff");

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
						<div className="relative w-full h-full flex items-center justify-center">
							<div
								className="w-0 h-0 border-l-[25px] border-l-transparent border-r-[25px] border-r-transparent border-b-[40px] -translate-y-[5px]"
								style={{ borderBottomColor: getArrowColor(pressedKeys.up) }}
							></div>
							<span
								className="absolute top-1/2 left-1/2 -translate-x-1/2 -translate-y-1/2 font-['Press_Start_2P'] text-lg z-10"
								style={{ color: getTextColor(pressedKeys.up), textShadow: pressedKeys.up ? '0 0 2px rgba(255,255,255,0.5)' : '0 0 2px rgba(0,0,0,0.5)' }}
							>
								w
							</span>
						</div>
					</div>
				</div>

			{/* Left, Down, Right Arrows */}
			<div className="flex items-center gap-6 -translate-y-[40px]">
				{/* Left Arrow */}
					<div className={getButtonClass(pressedKeys.left)}>
						<div className="relative w-full h-full flex items-center justify-center">
							<div
								className="w-0 h-0 border-t-[25px] border-t-transparent border-b-[25px] border-b-transparent border-r-[40px] -translate-x-[5px]"
								style={{ borderRightColor: getArrowColor(pressedKeys.left) }}
							></div>
							<span
								className="absolute top-1/2 left-1/2 -translate-x-1/2 -translate-y-1/2 font-['Press_Start_2P'] text-lg z-10"
								style={{ color: getTextColor(pressedKeys.left), textShadow: pressedKeys.left ? '0 0 2px rgba(255,255,255,0.5)' : '0 0 2px rgba(0,0,0,0.5)' }}
							>
								a
							</span>
						</div>
					</div>

					{/* Down Arrow */}
					<div className={getButtonClass(pressedKeys.down)}>
						<div className="relative w-full h-full flex items-center justify-center">
							<div
								className="w-0 h-0 border-l-[25px] border-l-transparent border-r-[25px] border-r-transparent border-t-[40px] translate-y-[5px]"
								style={{ borderTopColor: getArrowColor(pressedKeys.down) }}
							></div>
							<span
								className="absolute top-1/2 left-1/2 -translate-x-1/2 -translate-y-1/2 font-['Press_Start_2P'] text-lg z-10"
								style={{ color: getTextColor(pressedKeys.down), textShadow: pressedKeys.down ? '0 0 2px rgba(255,255,255,0.5)' : '0 0 2px rgba(0,0,0,0.5)' }}
							>
								s
							</span>
						</div>
					</div>

					{/* Right Arrow */}
					<div className={getButtonClass(pressedKeys.right)}>
						<div className="relative w-full h-full flex items-center justify-center">
							<div
								className="w-0 h-0 border-t-[25px] border-t-transparent border-b-[25px] border-b-transparent border-l-[40px] translate-x-[5px]"
								style={{ borderLeftColor: getArrowColor(pressedKeys.right) }}
							></div>
							<span
								className="absolute top-1/2 left-1/2 -translate-x-1/2 -translate-y-1/2 font-['Press_Start_2P'] text-lg z-10"
								style={{ color: getTextColor(pressedKeys.right), textShadow: pressedKeys.right ? '0 0 2px rgba(255,255,255,0.5)' : '0 0 2px rgba(0,0,0,0.5)' }}
							>
								d
							</span>
						</div>
					</div>
				</div>

				{/* ESC Key */}
				<div className="flex items-center gap-6 -translate-y-[30px]">
					<div className={getEscButtonClass(pressedKeys.esc)}>
						<span 
							className="font-['Press_Start_2P'] text-xl"
							style={{ color: pressedKeys.esc ? '#ffffff' : '#000000' }}
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
