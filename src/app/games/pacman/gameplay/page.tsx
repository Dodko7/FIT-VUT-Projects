"use client";
import PacmanGame from "../../../../components/games/pacman/pacmanView";
import Link from "next/link";
import { useRef } from "react";
import { GameController } from "../../../../components/games/pacman/pacmanControl";

export default function PacmanGamePage() {
	const pauseControll = useRef<GameController | null>(null);

	return (
		<div className="relative min-h-screen bg-black">
			<div className="absolute top-15 left-1/2 z-999 flex -translate-x-1/2 transform gap-4">
				<Link href="/">
					<button className="cursor-pointer bg-yellow-400 px-5 py-2 font-['Press_Start_2P'] text-white">
						Back
					</button>
				</Link>
				<button
					onClick={() => pauseControll.current?.model.togglePause()}
					className="cursor-pointer bg-yellow-400 px-5 py-2 font-['Press_Start_2P'] text-white">
					Pause / Resume
				</button>
			</div>
			<PacmanGame ref={pauseControll} />
		</div>
	);
}
