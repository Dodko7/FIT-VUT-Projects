import PacmanGame from "../../../components/games/pacman/pacman";
import Link from "next/link";

export default function PacmanGamePage() {
	return (
		<div className="relative min-h-screen bg-black">
			<Link href="/">
				<button className="absolute top-10 left-10 z-2 m-5 cursor-pointer bg-yellow-400 px-5 py-2 font-['Press_Start_2P'] text-white">Back</button>
			</Link>
			<PacmanGame />
		</div>
	);
}
