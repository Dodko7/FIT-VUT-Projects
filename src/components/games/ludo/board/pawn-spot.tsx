import PawnSpotHighlight from "~/lib/ludo/enum/pawn-spot-highlight";
import Pawn, { type PawnProps } from "../other/pawn";
import {
	FaArrowUp,
	FaArrowDown,
	FaArrowLeft,
	FaArrowRight,
} from "react-icons/fa";
import type { PawnSpotOnClicks } from "~/lib/ludo/types";

export type PawnSpotProps = {
	className: string;
	size: number;
	pawnProps?: PawnProps;
	hasPawn: boolean;
	highlight?: PawnSpotHighlight;
	onClick?: PawnSpotOnClicks,
	arrow?: "up" | "down" | "left" | "right";
};

/**
 * A pawn spot on the Ludo board, optionally displaying an arrow.
 * @param param0 Props for the pawn spot component.
 */
export default function PawnSpot({
	className,
	size,
	hasPawn,
	pawnProps,
	highlight,
	onClick,
	arrow,
}: PawnSpotProps) {
	const getArrow = () => {
		switch (arrow) {
			case "up":
				return <FaArrowUp className="text-2xl text-black" />;
			case "down":
				return <FaArrowDown className="text-2xl text-black" />;
			case "left":
				return <FaArrowLeft className="text-2xl text-black" />;
			case "right":
				return <FaArrowRight className="text-2xl text-black" />;
			default:
				return null;
		}
	};

	const getBackground = () => {
		switch (highlight) {
			case PawnSpotHighlight.AVALIABLE_MOVE:
				return "bg-green-400/70";
			case PawnSpotHighlight.AVALIABLE_PAWN:
				return "bg-yellow-400/70";
			case PawnSpotHighlight.SELECTED_PAWN:
				return "bg-blue-400/70";
			case PawnSpotHighlight.OPPONENT_PAWN:
				return "bg-red-400/70";
			default:
				return "";
		}
	};

	return (
		<div
			className={`flex items-center justify-center rounded-full border-4 ${highlight !== undefined ? getBackground() : className} relative ${onClick !== undefined ? "cursor-pointer hover:scale-105 transition-transform" : ""}`}
            onClick={onClick?.onClick}
			style={{ width: size, height: size }}
		>
			{!hasPawn && arrow && <div className="absolute">{getArrow()}</div>}
			{hasPawn && <Pawn {...pawnProps!} />}
		</div>
	);
}
