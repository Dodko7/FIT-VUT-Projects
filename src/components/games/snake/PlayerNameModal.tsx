/**
 * Player Name Modal Component
 * 
 * @author Igor Lacko
 * @description Modal pre zadanie mena hráča pred začatím hry
 */

"use client";

import { useState, useEffect, useRef } from "react";

interface PlayerNameModalProps {
	isOpen: boolean;
	onSubmit: (name: string) => void;
	onSkip: () => void;
}

export default function PlayerNameModal({
	isOpen,
	onSubmit,
	onSkip,
}: PlayerNameModalProps) {
	const [name, setName] = useState("");
	const [error, setError] = useState("");
	const inputRef = useRef<HTMLInputElement>(null);

	// Auto-focus input when modal opens
	useEffect(() => {
		if (isOpen && inputRef.current) {
			inputRef.current.focus();
		}
	}, [isOpen]);

	// Load saved name from localStorage
	useEffect(() => {
		const savedName = localStorage.getItem("snakePlayerName");
		if (savedName) {
			setName(savedName);
		}
	}, []);

	const handleSubmit = (e: React.FormEvent) => {
		e.preventDefault();
		
		const trimmedName = name.trim();
		
		// Validácia
		if (trimmedName.length === 0) {
			setError("Name cannot be empty");
			return;
		}

		if (trimmedName.length > 20) {
			setError("Name too long (max 20 characters)");
			return;
		}

		if (!/^[a-zA-Z0-9_-]+$/.test(trimmedName)) {
			setError("Only letters, numbers, _ and - allowed");
			return;
		}

		// Ulož do localStorage pre budúce použitie
		localStorage.setItem("snakePlayerName", trimmedName);
		
		onSubmit(trimmedName);
	};

	const handleSkip = () => {
		onSkip();
	};

	const handleKeyPress = (e: React.KeyboardEvent) => {
		if (e.key === "Escape") {
			handleSkip();
		}
	};

	if (!isOpen) return null;

	return (
		<div className="fixed inset-0 bg-black bg-opacity-90 flex items-center justify-center z-50">
			<div className="bg-gray-900 border-4 border-yellow-600 rounded-lg p-8 max-w-md w-full mx-4">
				<h2 className="font-['Press_Start_2P'] text-2xl text-yellow-400 text-center mb-6">
					Enter Your Name
				</h2>

				<form onSubmit={handleSubmit} className="space-y-4">
					<div>
						<input
							ref={inputRef}
							type="text"
							value={name}
							onChange={(e) => {
								setName(e.target.value);
								setError("");
							}}
							onKeyDown={handleKeyPress}
							placeholder="Your name..."
							maxLength={20}
							className="w-full px-4 py-3 bg-black border-2 border-yellow-600 rounded text-white font-['Press_Start_2P'] text-sm focus:outline-none focus:border-yellow-400 placeholder-gray-600"
						/>
						{error && (
							<p className="mt-2 font-['Press_Start_2P'] text-xs text-red-500">
								{error}
							</p>
						)}
					</div>

					<div className="flex flex-col gap-3">
						<button
							type="submit"
							className="w-full px-6 py-4 bg-gradient-to-b from-yellow-400 to-yellow-600 text-black font-['Press_Start_2P'] text-sm rounded-lg hover:from-yellow-500 hover:to-yellow-700 transition-all"
						>
							Start Game
						</button>

						<button
							type="button"
							onClick={handleSkip}
							className="w-full px-6 py-4 bg-gradient-to-b from-gray-600 to-gray-800 text-white font-['Press_Start_2P'] text-xs rounded-lg hover:from-gray-700 hover:to-gray-900 transition-all"
						>
							Play as Anonymous
						</button>
					</div>
				</form>

				<p className="mt-4 font-['Press_Start_2P'] text-xs text-gray-500 text-center">
					Your name will appear on the leaderboard
				</p>
			</div>
		</div>
	);
}
