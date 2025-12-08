"use client";
import PacmanGame from "~/components/games/pacman/pacmanView";
import Link from "next/link";
import { useRef, useState } from "react";
import { GameController } from "~/components/games/pacman/pacmanControl";
import ControlButtons from "~/components/games/pacman/buttons/controlButtons";

export default function PacmanGamePage() {
	const pauseControll = useRef<GameController | null>(null);
	const [isGameRunning, setIsGameRunning] = useState(false);

	return (
		<div className="relative min-h-screen bg-black">
			{isGameRunning && (
				<div className="absolute top-15 left-1/2 z-999 flex -translate-x-1/2 transform gap-4">
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


