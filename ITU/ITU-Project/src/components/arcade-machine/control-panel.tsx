"use client";

import ControlButton from "./control-button";
import ControlHandle from "./control-handle";

export default function ArcadeControlPanel() {
    return (
        <div
            className="flex w-full h-2/10 items-center justify-center control-panel-outer"
        >
            <div className="flex control-panel-inner w-full h-full items-center justify-between px-20">
                {/** Control buttons */}
                <div
                    className="flex flex-col items-center justify-center gap-4 py-7"
                >
                    <ControlButton isUp={true} onClick={() => console.log("Up button clicked")} />
                    <ControlButton isUp={false} onClick={() => console.log("Down button clicked")} />
                </div>
                {/** Control handle todo */}
                <ControlHandle />
            </div>
        </div>
    )
}