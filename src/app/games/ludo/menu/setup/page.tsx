"use client";

import { useState } from "react";
import type { CircularButtonProps } from "~/components/games/ludo/buttons/circular-button";
import CircularButton from "~/components/games/ludo/buttons/circular-button";
import LudoMenuHeader from "~/components/games/ludo/other/menu-header";
import Pawn from "~/components/games/ludo/other/pawn";

export default function LudoSetupPage() {
	// State vars that can't be represented by a form
	const [numberOfPlayers, setNumberOfPlayers] = useState<
		1 | 2 | 3 | 4 | null
	>(null);
	const [botsOn, setBotsOn] = useState<boolean>(false);

	// Props for number of players buttons
	const circularButtons: CircularButtonProps[] = [
		{
			text: "1",
			className: "ludo-red-button",
			onClick: () => setNumberOfPlayers(1 === numberOfPlayers ? null : 1),
			isActive: numberOfPlayers === 1,
		},
		{
			text: "2",
			className: "ludo-yellow-button",
			onClick: () => setNumberOfPlayers(numberOfPlayers === 2 ? null : 2),
			isActive: numberOfPlayers === 2,
		},
		{
			text: "3",
			className: "ludo-green-button",
			onClick: () => setNumberOfPlayers(3 === numberOfPlayers ? null : 3),
			isActive: numberOfPlayers === 3,
		},
		{
			text: "4",
			className: "ludo-blue-button",
			onClick: () => setNumberOfPlayers(4 === numberOfPlayers ? null : 4),
			isActive: numberOfPlayers === 4,
		},
	];

	return (
		<div className="flex h-full w-full flex-col items-center justify-start">
			{/** Header */}
			<LudoMenuHeader
				title="🎲 NEW GAME"
				className="h-1/11"
			/>
			{/** Rest of the page */}
			<form className="flex w-full flex-col items-center justify-start gap-10 px-25 py-15">
				{/** Game name input */}
				<div className="ludo-secondary flex w-full items-center justify-start rounded-[15px] px-10 py-4">
					<label
						htmlFor="gameName"
						className="mr-5 text-3xl"
					>
						Game Name:
					</label>
					<input
						type="text"
						id="gameName"
						name="gameName"
						className="flex-grow text-2xl focus:ring-0 focus:outline-none"
					/>
				</div>
				{/** Number of players buttons */}
				<div className="flex w-full items-center justify-between">
					<h2 className="ludo-form-text text-5xl">
						Number of Players:
					</h2>
					{circularButtons.map((props, index) => (
						<CircularButton
							key={index}
							{...props}
						/>
					))}
				</div>
				{/** Bot toggle */}
				<div className="flex w-full items-center justify-between">
					<h2 className="ludo-form-text text-5xl">
						Use bots as missing players:
					</h2>
					<div
						className="flex items-center justify-center gap-5 flex-grow"
					>
						<CircularButton
							text={botsOn ? "ON" : "OFF"}
							className={
								botsOn ? "ludo-green-button" : "ludo-red-button"
							}
							isActive={botsOn}
							onClick={() => setBotsOn(!botsOn)}
						/>
					</div>
				</div>
			</form>
		</div>
	);
}
