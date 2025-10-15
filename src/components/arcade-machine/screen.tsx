import Image from "next/image";

export default function ArcadeScreen() {
    return (
        <div
            className="relative h-1/2 flex flex-col w-full bg-none border-20 border-gray-600 rounded-lg"
        >
            <Image
            className="object-cover z-10 rounded-lg"
            src="/machine-bg.jpg"
            alt="Arcade Machine Screen"
            fill
            />
            <div
                className="absolute inset-0 z-20 flex flex-col items-center justify-between p-15 space-y-7 bg-none rounded-lg"
            >
                <div>dsad</div>
                <div>dsadsadsds</div>
                <div>dsadsadsds</div>
                <div>dsahuidshia</div>
            </div>
        </div>
    )
}