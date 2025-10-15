import ArcadeControlPanel from "./control-panel";
import ArcadeScreen from "./screen";

export default function ArcadeMachine() {
    return (
        <div
            className="z-10 flex flex-col items-center justify-top w-2/5 h-9/10 bg-none bg-opacity-60"
        >
            <ArcadeScreen />
            <div
                className="h-1/2 w-full bg-none flex flex-col items-center justify-top"
            >
                <div
                    className="h-1/5 w-1/10 bg-gray-600 border-y-2 border-black"
                />
                <ArcadeControlPanel />
            </div>
        </div>
    )
}