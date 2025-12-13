/**
 * Snake Gameplay Page
 * 
 * @author Igor Lacko
 * @description Hlavná herná stránka s funkčnou hrou
 */

"use client";

import { useEffect, useRef, useState } from "react";
import { useRouter } from "next/navigation";
import { useSnakeGame } from "~/contexts/SnakeGameContext";
import { SnakeGameController } from "~/components/games/snake/snakeControl";
import { getSoundManager } from "~/components/games/snake/snakeSound";
import TouchControls from "~/components/games/snake/TouchControls";
import PlayerNameModal from "~/components/games/snake/PlayerNameModal";
import type { Direction } from "~/components/games/snake/snakeModel";

export default function SnakeGameplayPage() {
	const { currentGameId, setCurrentGameId, gameType, level, playerName, setPlayerName, resetAll } = useSnakeGame();
	const router = useRouter();
	
	const canvasRef = useRef<HTMLCanvasElement>(null);
	const controllerRef = useRef<SnakeGameController | null>(null);
	const isInitializedRef = useRef(false);
	const soundManager = getSoundManager();
	
	const [score, setScore] = useState(0);
	const [isGameOver, setIsGameOver] = useState(false);
	const [isPaused, setIsPaused] = useState(false);
	const [isLoading, setIsLoading] = useState(true);
	const [isMuted, setIsMuted] = useState(soundManager.isSoundMuted());
	const [isMobile, setIsMobile] = useState(false);
	const [actualGameType, setActualGameType] = useState<string>(gameType);
	const [showNameModal, setShowNameModal] = useState(false);

	// Detekuj mobilné zariadenie
	useEffect(() => {
		const checkMobile = () => {
			setIsMobile(window.innerWidth < 768 || 'ontouchstart' in window);
		};
		checkMobile();
		window.addEventListener('resize', checkMobile);
		return () => window.removeEventListener('resize', checkMobile);
	}, []);

	// Redirect ak nemáme game ID
	useEffect(() => {
		if (!currentGameId) {
			router.push("/games/snake");
		}
	}, [currentGameId, router]);

	// Inicializácia hry
	useEffect(() => {
		if (!currentGameId || !canvasRef.current) return;
		
		// Prevent double initialization in development (React Strict Mode)
		// Check both the ref flag AND if controller already exists
		if (isInitializedRef.current || controllerRef.current) {
			return;
		}

		// SET FLAG IMMEDIATELY to prevent race condition with async initialization
		isInitializedRef.current = true;

		const canvas = canvasRef.current;
		let controller: SnakeGameController | null = null;
		
		// Fetch game from database to get the correct gameType
		const initializeGame = async () => {
			try {
				const response = await fetch(`/api/snake/game?gameId=${currentGameId}`);
				const result = await response.json() as {
					success: boolean;
					data?: {
						gameType: string;
						level: number;
						playerName: string;
					};
					error?: string;
				};
				
				if (!result.success || !result.data) {
					console.error("Failed to load game from database:", result.error);
					setIsLoading(false);
					return;
				}
				
				const { gameType: dbGameType, level: dbLevel, playerName: dbPlayerName } = result.data;
				setActualGameType(dbGameType);
				
				controller = new SnakeGameController(
					canvas,
					{
						gameId: currentGameId,
						gameType: dbGameType as "CLASSIC" | "BOX" | "CAMPAIGN",
						level: dbLevel,
						playerName: dbPlayerName,
						gridSize: 20,
						cellSize: 25,
						autoSaveInterval: 5000,
					},
				{
					onGameOver: (finalScore) => {
						setScore(finalScore);
						setIsGameOver(true);
						// Show name modal if player is still Anonymous
						if (!playerName || playerName === "Anonymous") {
							setShowNameModal(true);
						}
					},
					onScoreChange: (newScore) => {
						setScore(newScore);
					},
					onError: (error) => {
						console.error("Game error:", error);
					},
				}
			);

			controllerRef.current = controller;			// Pokús sa načítať uložený stav
			const loaded = await controller.loadSavedState();
			
			// Start the game
			controller.start();
			setIsLoading(false);
		} catch (error) {
			console.error("Error initializing game:", error);
			setIsLoading(false);
		}
	};
	
	void initializeGame();	// Cleanup - DO NOT reset isInitializedRef here as it causes double init in Strict Mode
	return () => {
		if (controller) {
			controller.stop();
		}
		if (controllerRef.current) {
			controllerRef.current.stop();
			controllerRef.current = null;
		}
	};
}, [currentGameId]);	// Handle name submission after game over
	const handleNameSubmit = async (name: string) => {
		setPlayerName(name);
		setShowNameModal(false);
		
		// Update player name in database
		if (currentGameId) {
			try {
				await fetch("/api/snake/game", {
					method: "PATCH",
					headers: { "Content-Type": "application/json" },
					body: JSON.stringify({
						gameId: currentGameId,
						playerName: name,
					}),
				});
			} catch (error) {
				console.error("Failed to update player name:", error);
			}
		}
	};

	const handleNameSkip = () => {
		setShowNameModal(false);
	};

	// Handle back to menu
	const handleBackToMenu = () => {
		if (controllerRef.current) {
			controllerRef.current.stop();
			controllerRef.current = null;
		}
		isInitializedRef.current = false;
		// Only reset game ID, keep gameType and level settings
		setCurrentGameId(null);
		router.push("/games/snake");
	};

	// Handle play again - restart game with same settings
	const handlePlayAgain = async () => {
		try {
			// Stop current controller
			if (controllerRef.current) {
				controllerRef.current.stop();
				controllerRef.current = null;
			}
			
			// Get current game settings
			const response = await fetch(`/api/snake/game?gameId=${currentGameId}`);
			const result = await response.json() as {
				success: boolean;
				data?: {
					gameType: string;
					level: number;
					playerName: string;
				};
			};
			
			if (!result.success || !result.data) {
				console.error("Failed to load game settings");
				return;
			}
			
			const { gameType: dbGameType, level: dbLevel, playerName: dbPlayerName } = result.data;
			
			// Create new game with same settings
			const createResponse = await fetch("/api/snake/game", {
				method: "POST",
				headers: { "Content-Type": "application/json" },
				body: JSON.stringify({
					playerName: dbPlayerName,
					gameType: dbGameType,
					level: dbLevel,
				}),
			});
			
			const createResult = await createResponse.json();
			
			if (createResult.success) {
				// Update game ID in context
				setCurrentGameId(createResult.data.id);
				
				// Reset local state
				setScore(0);
				setIsGameOver(false);
				setIsPaused(false);
				setIsLoading(true);
				isInitializedRef.current = false;
				
				// The useEffect will reinitialize the game with the new ID
			} else {
				console.error("Failed to create new game:", createResult.error);
			}
		} catch (error) {
			console.error("Error restarting game:", error);
		}
	};

	// Handle view leaderboard
	const handleViewLeaderboard = () => {
		if (controllerRef.current) {
			controllerRef.current.stop();
			controllerRef.current = null;
		}
		isInitializedRef.current = false;
		// Don't call resetAll() here - keep game context for potential return
		router.push("/games/snake/leaderboard");
	};

	// Handle pause toggle
	const handlePauseToggle = () => {
		if (!controllerRef.current || isGameOver) return;

		if (isPaused) {
			controllerRef.current.resume();
		} else {
			controllerRef.current.pause();
		}
		setIsPaused(!isPaused);
	};

	// Handle sound toggle
	const handleSoundToggle = () => {
		const newMutedState = soundManager.toggleMute();
		setIsMuted(newMutedState);
	};

	// Handle touch control direction
	const handleTouchDirection = (direction: Direction) => {
		if (controllerRef.current && !isGameOver) {
			controllerRef.current.setDirection(direction);
		}
	};

	if (!currentGameId) {
		return (
			<div className="flex min-h-screen items-center justify-center bg-black">
				<p className="font-['Press_Start_2P'] text-yellow-400">
					Loading...
				</p>
			</div>
		);
	}

	return (
		<div className="flex min-h-screen flex-col items-center justify-center bg-black text-white">
			{/* HUD Top Bar */}
			<div className="absolute top-8 left-0 right-0 flex justify-between px-8">
				<button
					onClick={handleBackToMenu}
					className="px-6 py-3 bg-gradient-to-b from-[#fef3c7] to-[#fde047] text-black font-['Press_Start_2P'] text-sm rounded-lg hover:from-[#fde047] hover:to-[#facc15] transition-all"
				>
					Menu
				</button>


			<div className="flex gap-4 items-center">
				<div className="font-['Press_Start_2P'] text-sm text-yellow-400">
					Score: {score}
				</div>
				<button
					onClick={handleSoundToggle}
					className="px-4 py-3 bg-gradient-to-b from-purple-400 to-purple-600 text-white font-['Press_Start_2P'] text-sm rounded-lg hover:from-purple-500 hover:to-purple-700 transition-all"
					title={isMuted ? "Unmute Sound" : "Mute Sound"}
				>
					{isMuted ? "🔇" : "🔊"}
				</button>
				<button
					onClick={handlePauseToggle}
					disabled={isGameOver}
					className={`px-6 py-3 font-['Press_Start_2P'] text-sm rounded-lg transition-all ${
						isGameOver
							? "bg-gray-600 text-gray-400 cursor-not-allowed"
							: isPaused
								? "bg-gradient-to-b from-green-400 to-green-600 text-white hover:from-green-500 hover:to-green-700"
								: "bg-gradient-to-b from-yellow-400 to-yellow-600 text-black hover:from-yellow-500 hover:to-yellow-700"
					}`}
				>
					{isPaused ? "Resume" : "Pause"}
				</button>
			</div>
		</div>

		{/* Game Canvas */}
		<div className="flex flex-col lg:flex-row items-center gap-8">
			<div className="flex flex-col items-center gap-4">
				<h1 className="font-['Press_Start_2P'] text-3xl snake-gradient-text mb-4">
					{actualGameType.charAt(0) + actualGameType.slice(1).toLowerCase()} Mode
				</h1>

				<div className="relative">
					<canvas
						ref={canvasRef}
						className="border-4 border-yellow-600 rounded-lg shadow-2xl"
					/>

					{/* Loading Overlay */}
					{isLoading && (
						<div className="absolute inset-0 bg-black bg-opacity-80 flex items-center justify-center rounded-lg">
							<p className="font-['Press_Start_2P'] text-yellow-400">
								Loading...
							</p>
						</div>
					)}
				</div>

			{/* Controls Info */}
			<div className="mt-4 font-['Press_Start_2P'] text-xs text-gray-400 text-center">
				<p>Arrow Keys / WASD - Move</p>
				<p>Space / P / ESC - Pause</p>
			</div>
			</div>

			{/* Mobile Touch Controls */}
			{isMobile && (
				<div className="lg:ml-8">
					<TouchControls
						onDirectionChange={handleTouchDirection}
						onPause={handlePauseToggle}
						disabled={isGameOver || isLoading}
					/>
				</div>
			)}
		</div>

		{/* Game Over Modal - only show if name modal is not open */}
		{isGameOver && !showNameModal && (
				<div className="fixed inset-0 bg-black bg-opacity-90 flex items-center justify-center z-50">
					<div className="bg-gray-900 border-4 border-red-600 rounded-lg p-8 max-w-md w-full mx-4">
						<h2 className="font-['Press_Start_2P'] text-3xl text-red-500 text-center mb-6">
							GAME OVER
						</h2>

						<div className="bg-black border-2 border-yellow-600 rounded p-4 mb-6">
							<p className="font-['Press_Start_2P'] text-sm text-yellow-400 mb-2">
								Final Score:
							</p>
							<p className="font-['Press_Start_2P'] text-4xl text-white text-center">
								{score}
							</p>
						</div>

						<div className="space-y-3">
							<button
								onClick={handlePlayAgain}
								className="w-full px-6 py-4 bg-gradient-to-b from-green-400 to-green-600 text-white font-['Press_Start_2P'] text-sm rounded-lg hover:from-green-500 hover:to-green-700 transition-all"
							>
								Play Again
							</button>

							<button
								onClick={handleViewLeaderboard}
								className="w-full px-6 py-4 bg-gradient-to-b from-blue-400 to-blue-600 text-white font-['Press_Start_2P'] text-sm rounded-lg hover:from-blue-500 hover:to-blue-700 transition-all"
							>
								Leaderboard
							</button>

						<button
							onClick={handleBackToMenu}
							className="w-full px-6 py-4 bg-gradient-to-b from-gray-400 to-gray-600 text-white font-['Press_Start_2P'] text-sm rounded-lg hover:from-gray-500 hover:to-gray-700 transition-all"
						>
							Main Menu
						</button>
					</div>
				</div>
			</div>
			)}

			{/* Player Name Modal - shown after game over */}
			<PlayerNameModal
				isOpen={showNameModal}
				onSubmit={handleNameSubmit}
				onSkip={handleNameSkip}
			/>
		</div>
	);
}