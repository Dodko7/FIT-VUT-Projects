import Image from "next/image";
import MenuTitle from "./menu-title";

export default function ArcadeScreen() {
	return (
		<div className="relative mt-8 flex h-4/10 w-5/6 flex-col bg-none">
			<Image
				className="z-10 object-cover"
				src="/machine-bg.png"
				alt="Arcade Machine Screen"
				fill
			/>
			<div className="z-20 flex h-full w-full flex-col items-center justify-start rounded-lg bg-none">
				<h1 className="cursor-default pt-5 font-['Press_Start_2P'] text-2xl text-yellow-400">
					Arcade Machine
				</h1>
				<div
					className="flex flex-col items-center justify-center gap-10 pt-11 pb-7"
				>
					<MenuTitle
						title="Pacman"
						href="/games/pacman/menu"
					/>

					<MenuTitle
						title="Snake"
						href="/games/snake"
					/>

					<MenuTitle
						title="Ludo"
						href="/games/ludo/menu"
					/>

					<MenuTitle
						title="Settings"
						href="/settings"
					/>
				</div>
			</div>
		</div>
	);
}
