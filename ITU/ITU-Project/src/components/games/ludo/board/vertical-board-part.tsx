import type { BoardPartProps } from "~/lib/ludo/types";
import PawnSpot from "./pawn-spot";

export default function VerticalBoardPart({ pawnSpotProps, extraClassNames }: BoardPartProps) {
    return (
        <div
            className={`grid grid-rows-5 grid-cols-3 place-items-center ${extraClassNames || ""}`}
        >
            {pawnSpotProps.map((props, index) => (
                <PawnSpot key={index} {...props} />
            ))}
        </div>
    )
}