"use client";

import { useState } from "react";
import type { CircularButtonProps } from "~/components/games/ludo/buttons/circular-button";
import CircularButton from "~/components/games/ludo/buttons/circular-button";
import LudoMenuHeader from "~/components/games/ludo/other/menu-header";
import type { PlayerNameInputProps } from "~/components/games/ludo/other/player-name-input";
import PlayerNameInput from "~/components/games/ludo/other/player-name-input";
import {
	MENU_BLUE_PAWN_COLOR,
	MENU_RED_PAWN_COLOR,
	MENU_YELLOW_PAWN_COLOR,
	MENU_GREEN_PAWN_COLOR,
} from "~/lib/ludo/constants";
import { useRouter } from "next/navigation";
import LudoMenuButton from "~/components/games/ludo/buttons/menu-button";
import ErrorDiv from "~/components/games/ludo/other/error-div";
import { CreateNewGame } from "~/lib/ludo/client-api/new-game";
import { Color } from "@prisma/client";
import ColorPicker from "~/components/games/ludo/other/color-picker";

export default function LudoSetupPage() {
	// State vars that can't be represented by a form
	const [numberOfPlayers, setNumberOfPlayers] = useState<
		1 | 2 | 3 | 4 | null
	>(null);
	const [botsOn, setBotsOn] = useState<boolean>(false);
	const [selectedColor, setSelectedColor] = useState<Color>(Color.RED);
	const [error, setError] = useState<string | undefined>(undefined);

	// Router to go back/load game
	const router = useRouter();

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

	// Props for player name inputs
	const playerNameInputs: Map<Color, PlayerNameInputProps> = new Map([
		[
			Color.RED,
			{
				pawnColor: MENU_RED_PAWN_COLOR,
				inputDivClassName: "ludo-player-input-red",
				isVisible: true,
			},
		],
		[
			Color.YELLOW,
			{
				pawnColor: MENU_YELLOW_PAWN_COLOR,
				inputDivClassName: "ludo-player-input-yellow",
				isVisible: true,
			},
		],
		[
			Color.BLUE,
			{
				pawnColor: MENU_BLUE_PAWN_COLOR,
				inputDivClassName: "ludo-player-input-blue",
				isVisible: true,
			},
		],
		[
			Color.GREEN,
			{
				pawnColor: MENU_GREEN_PAWN_COLOR,
				inputDivClassName: "ludo-player-input-green",
				isVisible: true,
			},
		],
	]);

	return (
		<div className="flex min-h-full w-full flex-col items-center justify-start overflow-y-auto">
			{/** Header */}
			<LudoMenuHeader
				title="🎲 NEW GAME"
				className="h-1/11"
			/>
			{/** Rest of the page */}
			<form
				className="flex w-full flex-grow flex-col items-center justify-between px-25 py-15"
				onSubmit={async (e) => {
					// Form stuff
					e.preventDefault();
					const formData = new FormData(e.currentTarget);

					// Pass to handler
					const res = await CreateNewGame(
						formData,
						numberOfPlayers,
						botsOn,
						selectedColor,
					);
					if (res.success) {
						console.log("New game created with ID:", res); // Debug log
						const newGameId = res.value;
						// Auto-join as selected color
						router.push(
							`/games/ludo/gameplay/${newGameId}?color=${selectedColor}`,
						);
					} else {
						setError(res.error!);
					}
				}}
			>
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
					<div className="flex flex-grow items-center justify-center gap-5">
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
				{/** Player color picker */}
				<ColorPicker
					currentColor={selectedColor}
					onColorChange={(newColor) => setSelectedColor(newColor)}
				/>
				{/** Player name input */}
				<PlayerNameInput
					{...playerNameInputs.get(selectedColor)!}
				/>
				{/** Potential error todo */}
				{error && (
					<ErrorDiv
						message={error}
						onClose={() => setError(undefined)}
						isVisible={error !== undefined}
					/>
				)}

				{/** Submit and back buttons */}
				<div className="flex w-full items-center justify-center gap-25">
					<LudoMenuButton
						text="Start game"
						type="submit"
					/>
					<LudoMenuButton
						text="Go back"
						onClick={() => router.push("/games/ludo/menu")}
					/>
				</div>
			</form>
		</div>
	);
}
