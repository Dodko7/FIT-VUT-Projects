import Image from "next/image";
import Link from "next/link";

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
                <h1
                    className="text-yellow-400 text-4xl font-['Press_Start_2P'] cursor-default"
                >Arcade Machine</h1>
                <h2
                className="text-yellow-400 text-3xl font-['Press_Start_2P'] cursor-pointer"
                >
                <Link href="/games/pacman" className="text-yellow-400">
                Pacman
                </Link>
                </h2>
                <h2
                    className="text-yellow-400 text-3xl font-['Press_Start_2P'] cursor-pointer"
                >Ludo</h2>
                <h2
                    className="text-yellow-400 text-3xl font-['Press_Start_2P'] cursor-pointer"
                >Settings</h2>
            </div>
        </div>
    )
}