import Pawn, { type PawnProps } from "../other/pawn";

export type PawnSpotProps = {
	className: string;
    size: number;
	pawnProps?: PawnProps;
	hasPawn: boolean;
};

export default function PawnSpot({
	className,
    size,
	hasPawn,
	pawnProps,
}: PawnSpotProps) {
	return (
		<div
			className={`flex items-center justify-center rounded-full border-4 ${className}`}
            style={{ width: size, height: size }}
		>
			{hasPawn && <Pawn {...pawnProps!} />}
		</div>
	);
}
