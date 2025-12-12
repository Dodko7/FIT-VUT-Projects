import type PawnSpotHighlight from "~/lib/ludo/enum/pawn-spot-highlight";
import type { PawnProps } from "../other/pawn";
import PawnSpot from "./pawn-spot";
import type { HighlightedPawnSpot, PawnSpotOnClicks } from "~/lib/ludo/types";
import { Color } from "@prisma/client";
import {
	BLUE_PAWN_START_POSITIONS,
	GREEN_PAWN_START_POSITIONS,
	RED_PAWN_START_POSITIONS,
	YELLOW_PAWN_START_POSITIONS,
} from "~/lib/ludo/constants";

export type PlayerStartProps = {
	divClassName: string;
	pawnSpotClassName: string;
	pawnsPresent: boolean[];
	pawnProps: PawnProps;
	highlights: HighlightedPawnSpot[];
	onClicks: PawnSpotOnClicks[];
	color: Color;
};

export default function PlayerStart({
	divClassName,
	pawnSpotClassName,
	pawnsPresent,
	pawnProps,
	highlights,
	onClicks,
	color,
}: PlayerStartProps) {
	const indexToPosition = (index: number) => {
		// 0 | 1 | 2 | 3 -> -1 | -2 | -3 | -4
		let position = -1 * (index + 1);
		switch (color) {
			case Color.RED:
				return position;
			case Color.YELLOW:
				return position - 4;
			case Color.GREEN:
				return position - 8;
			case Color.BLUE:
				return position - 12;
		}
	};


	return (
		<div
			className={`flex items-center justify-center border-y-4 ${divClassName}`}
		>
			{/** Grid of pawn spots */}
			<div className="grid h-full w-full grid-cols-2 place-items-center p-4">
				{pawnsPresent.map((hasPawn, index) => (
					<PawnSpot
						key={index}
						className={pawnSpotClassName}
						hasPawn={hasPawn}
						size={80}
						pawnProps={pawnProps}
						highlight={
							highlights.find(
								(h) => h.position === indexToPosition(index),
							)?.highlight
						}
						onClick={
							onClicks.find(
								(o) => o.position === indexToPosition(index),
							)
						}
					/>
				))}
			</div>
		</div>
	);
}
