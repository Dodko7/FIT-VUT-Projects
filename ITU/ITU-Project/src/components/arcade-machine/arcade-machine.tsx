import ArcadeControlPanel from "./control-panel";
import ArcadeScreen from "./screen";
import Image from "next/image";

export default function ArcadeMachine() {
	return (
		<div className="relative z-1 flex h-9/10 w-1/4 flex-col items-center justify-start">
            {/** Background pattern */}
            <Image
                className="absolute inset-0 z-1 object-cover w-full h-full"
                src="/machine-pattern.png"
                alt="Arcade Machine Background Pattern"
                fill
                priority
            />
            {/** Div with content */}
            <div className="z-10 flex h-full w-full flex-col items-center justify-start">
                {/** Screen */}
                <ArcadeScreen />
                {/** Controls */}
                <ArcadeControlPanel />
                {/** Fill out the rest and put the image there again*/}
                <div
                    className="flex relative h-4/10 w-full"
                >
                    <Image
                        className="absolute inset-0 object-cover w-full h-full"
                        src="/machine-pattern.png"
                        alt="Arcade Machine Background Pattern"
                        fill
                        priority
                    />
                </div>
            </div>
		</div>
	);
}
