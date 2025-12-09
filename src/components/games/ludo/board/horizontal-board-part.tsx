import type { BoardPartProps } from "~/lib/ludo/types";
import PawnSpot from "./pawn-spot";

export default function HorizontalBoardPart({ pawnSpotProps }: BoardPartProps) {
    return (
        <div
            className="grid grid-rows-3 grid-cols-5 place-items-center"
        >
            {pawnSpotProps.map((props, index) => (
                <PawnSpot key={index} {...props} />
            ))}
        </div>
    )
}