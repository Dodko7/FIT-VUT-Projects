import Pawn, { type PawnProps } from "../other/pawn";
import { FaArrowUp, FaArrowDown, FaArrowLeft, FaArrowRight } from "react-icons/fa";

export type PawnSpotProps = {
	className: string;
    size: number;
	pawnProps?: PawnProps;
	hasPawn: boolean;
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
    arrow,
}: PawnSpotProps) {
    const getArrow = () => {
        switch (arrow) {
            case "up": return <FaArrowUp className="text-black/50 text-2xl" />;
            case "down": return <FaArrowDown className="text-black/50 text-2xl" />;
            case "left": return <FaArrowLeft className="text-black/50 text-2xl" />;
            case "right": return <FaArrowRight className="text-black/50 text-2xl" />;
            default: return null;
        }
    };

	return (
		<div
			className={`flex items-center justify-center rounded-full border-4 ${className} relative`}
            style={{ width: size, height: size }}
		>
            {!hasPawn && arrow && <div className="absolute">{getArrow()}</div>}
			{hasPawn && <Pawn {...pawnProps!} />}
		</div>
	);
}
