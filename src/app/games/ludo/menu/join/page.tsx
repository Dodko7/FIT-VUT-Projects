"use client";

import { useRouter } from "next/navigation";
import { useState } from "react";
import LudoMenuButton from "~/components/games/ludo/buttons/menu-button";
import LudoMenuHeader from "~/components/games/ludo/other/menu-header";
import { FaNetworkWired, FaGamepad } from "react-icons/fa";

/**
 * Page for joining an existing ludo game.
 */
export default function LudoJoinPage() {
	const router = useRouter();
	const [ipAddress, setIpAddress] = useState("");
	const [gameId, setGameId] = useState("");

	return (
		<div className="flex h-full w-full flex-col items-center justify-between py-10">
			<div className="flex w-full flex-col items-center gap-10">
				<LudoMenuHeader title="JOIN GAME" />
				<div className="flex w-11/12 flex-col items-center gap-8 px-8">
					{/* IP Input */}
					<div className="ludo-player-input-blue flex h-20 w-full items-center justify-start rounded-[40px] px-4 py-2">
						<FaNetworkWired
							size={40}
							className="ml-2"
						/>
						<label className="mx-5 text-3xl whitespace-nowrap">
							Host IP or name:
						</label>
						<input
							type="text"
							value={ipAddress}
							onChange={(e) => setIpAddress(e.target.value)}
							placeholder="e.g. mnau.com"
							className="w-full bg-transparent text-3xl placeholder-blue-800/50 focus:ring-0 focus:outline-none"
						/>
					</div>

					{/* Game ID Input */}
					<div className="ludo-player-input-yellow flex h-20 w-full items-center justify-start rounded-[40px] px-4 py-2">
						<FaGamepad
							size={45}
							className="ml-2"
						/>
						<label className="mx-5 text-3xl whitespace-nowrap">
							Game name:
						</label>
						<input
							type="text"
							value={gameId}
							onChange={(e) => setGameId(e.target.value)}
							placeholder="e.g. epic-friday-game"
							className="w-full bg-transparent text-3xl placeholder-yellow-800/50 focus:ring-0 focus:outline-none"
						/>
					</div>
				</div>
			</div>

			{/* Action Buttons */}
			<div className="flex w-full flex-row justify-center gap-10">
				<LudoMenuButton
					text="Join"
					onClick={() => console.log(`Joining game ${gameId} at ${ipAddress}`)}
				/>
				<LudoMenuButton
					text="Back"
					onClick={() => router.back()}
				/>
			</div>
		</div>
	);
}
