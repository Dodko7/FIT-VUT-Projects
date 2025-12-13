import type { Color } from "@prisma/client";
import LudoMenuButton from "../buttons/menu-button";
import Dice from "../other/dice";

export type middleProps = {
	diceNumber: number;
	playerColor: Color;
	playerName: string;
	onButtonClick?: (() => Promise<void>) | (() => void);
	text?: string;
	isRolling: boolean;
	active: boolean;
};

export default function Middle({
	diceNumber,
	playerColor,
	playerName,
	onButtonClick,
	text,
	isRolling,
	active,
}: middleProps) {
	let colorClass = "";
	switch (playerColor) {
		case "RED":
			colorClass = "ludo-player-red-text";
			break;
		case "GREEN":
			colorClass = "ludo-player-green-text";
			break;
		case "BLUE":
			colorClass = "ludo-player-blue-text";
			break;
		case "YELLOW":
			colorClass = "ludo-player-yellow-text";
			break;
	}

	return (
		<div className="board-middle flex h-full w-full flex-col items-center justify-between py-4">
			<div className={`text-4xl font-bold ${colorClass}`}>
				{playerName}'s Turn
			</div>
			{/** Dice component */}
			<Dice
				lastRoll={diceNumber as any}
				isRolling={isRolling}
			/>
			{active && text && onButtonClick && (
				<LudoMenuButton
					text={text}
					onClick={onButtonClick}
					padding="px-1 py-2"
				/>
			)}
			{/** Padding */}
			{!active && <div className="h-16 px-1 py-2"></div>}
		</div>
	);
}
