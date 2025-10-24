import Image from "next/image";
import Link from "next/link";
import MenuTitle from "./menu-title";

export default function ArcadeScreen() {
	return (
		<div className="relative flex h-1/2 w-full flex-col rounded-lg border-20 border-gray-600 bg-none">
			<Image
				className="z-10 rounded-lg object-cover"
				src="/machine-bg.jpg"
				alt="Arcade Machine Screen"
				fill
			/>
			<div className="absolute inset-0 z-20 flex flex-col items-center justify-between space-y-7 rounded-lg bg-none p-15">
				<h1 className="cursor-default font-['Press_Start_2P'] text-4xl text-yellow-400">
					Arcade Machine
				</h1>
				<MenuTitle
					title="Pacman"
					href="/games/pacman"
				/>

                <MenuTitle
                    title="Snake"
                    href="/games/snake"
                />

				<MenuTitle
					title="Ludo"
					href="/games/ludo/config"
				/>

				<MenuTitle
					title="Settings"
					href="/settings"
				/>
			</div>
		</div>
	);
}
