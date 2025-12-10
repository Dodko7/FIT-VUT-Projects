/**
 * @brief pacman gameplay with level selection before the game
 */

"use client";
import PacmanGame from "~/components/games/pacman/pacmanView";
import { useRef, useState } from "react";
import { GameController } from "~/components/games/pacman/pacmanControl";
import ControlButtons from "~/components/games/pacman/buttons/controlButtons";

/**
 * @brief list all levels and start a game for a chosen level
 * @returns pacman game
 */
export default function PacmanGamePage() {
	const pauseControll = useRef<GameController | null>(null);
	const [isGameRunning, setIsGameRunning] = useState(false);

	return (
		<div className="relative min-h-screen bg-black">
			{isGameRunning && (
				<div className="absolute top-15 left-1/2 z-10 flex -translate-x-1/2 transform gap-4">
					<ControlButtons 
						text = "Back"
                    	link = "/games/pacman/menu"
					/>
					<ControlButtons 
						text = "Pause / Resume"
                    	onClick={() => pauseControll.current?.model.togglePause()}
					/>
				</div>
			)}	
			<PacmanGame 
				ref={pauseControll} 
				onGameStart={() => setIsGameRunning(true)} 
			/>
		</div>
	);
}