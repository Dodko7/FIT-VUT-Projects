"use client";

import LudoMenuButton from "../buttons/menu-button";
import type { GameOverPageProps } from "~/lib/ludo/types";

export default function GameOverPage({ gameName, onQuit }: GameOverPageProps) {
	return (
		<div className="ludo-bg flex h-screen w-screen flex-col items-center justify-center gap-10">
			{/* Title */}
			<div className="mt-10 flex flex-col items-center gap-4">
				<h1 className="ludo-text-primary text-7xl text-[#ffc916]">
					Game {gameName} is over!
				</h1>
			</div>
			{/* Menu */}
			<div className="mt-10 flex flex-row gap-8">
				<LudoMenuButton
					text="Menu"
					onClick={onQuit}
				/>
			</div>
		</div>
	);
}
