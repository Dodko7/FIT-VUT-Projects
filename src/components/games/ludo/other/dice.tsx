import type { DiceProps } from "~/lib/ludo/types";

/**
 * Component representing a dice.
 * @param param0 Last roll (default 6) and rolling state.
 * @returns The Dice component.
 */
export default function Dice({ lastRoll, isRolling }: DiceProps) {
	const roll = lastRoll ?? 6;

	const renderDots = (num: number) => {
		const dots = [];
		// Positions for a 3x3 grid
		// 0 1 2
		// 3 4 5
		// 6 7 8

		const dotPositions: Record<number, number[]> = {
			1: [4],
			2: [0, 8],
			3: [0, 4, 8],
			4: [0, 2, 6, 8],
			5: [0, 2, 4, 6, 8],
			6: [0, 2, 3, 5, 6, 8],
		};

		const activeDots = dotPositions[num] || [];

		for (let i = 0; i < 9; i++) {
			dots.push(
				<div
					key={i}
					className={`size-2 rounded-full ${activeDots.includes(i) ? "bg-black" : "bg-transparent"}`}
				/>,
			);
		}
		return dots;
	};

	return (
		<div
			className={`flex size-25 items-center justify-center rounded-xl border-4 border-black bg-white shadow-lg ${isRolling ? "dice-rolling" : ""}`}
		>
			<div className="grid h-full w-full grid-cols-3 grid-rows-3 place-items-center gap-1 p-2">
				{renderDots(roll)}
			</div>
		</div>
	);
}
