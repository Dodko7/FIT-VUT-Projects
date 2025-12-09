import type { PawnProps } from "../other/pawn";
import PawnSpot from "./pawn-spot";

export type PlayerStartProps = {
    divClassName: string;
    pawnSpotClassName: string;
    pawnsPresent: boolean[];
    pawnProps: PawnProps;
};

export default function PlayerStart({
    divClassName,
    pawnSpotClassName,
    pawnsPresent,
    pawnProps
}: PlayerStartProps) {
    return (
        <div
            className={`flex items-center justify-center border-y-4 ${divClassName}`}
        >
            {/** Grid of pawn spots */}
            <div
                className="grid grid-cols-2 place-items-center p-4 w-full h-full"
            >
                {pawnsPresent.map((hasPawn, index) => (
                    <PawnSpot
                        key={index}
                        className={pawnSpotClassName}
                        hasPawn={hasPawn}
                        size={80}
                        pawnProps={pawnProps}
                    />
                ))}
            </div>
        </div>
    )
}