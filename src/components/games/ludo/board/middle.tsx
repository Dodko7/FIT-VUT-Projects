import type { Color } from "@prisma/client";
import LudoMenuButton from "../buttons/menu-button";
import Dice from "../other/dice";

export type middleProps = {
    diceNumber: number;
    playerColor: Color;
    playerName: string;
    onRollDice: () => void;
    isRolling: boolean;
}

export default function Middle({ diceNumber, playerColor, playerName, onRollDice, isRolling }: middleProps) {
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
        <div className="flex flex-col items-center justify-between w-full h-full board-middle py-4">
            <div className={`text-4xl font-bold ${colorClass}`}>
                {playerName}'s Turn
            </div>
            {/** Dice component */}
            <Dice lastRoll={diceNumber as any} isRolling={isRolling} />
            <LudoMenuButton
                text="Roll dice"
                onClick={onRollDice}
                padding="px-1 py-2"
            />
        </div>
    )
}