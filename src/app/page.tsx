import Image from "next/image";
import ArcadeMachine from "~/components/arcade-machine/arcade-machine";

export default function HomePage() {
	return (
		<main className="fixed inset-0 flex h-screen w-screen flex-col items-center justify-center bg-gradient-to-b from-[#2e026d] to-[#15162c] text-white">
			<Image
				className="absolute inset-0 z-0 h-full w-full object-cover"
				src="/arcade-bg.png"
				alt="Background"
				fill
			/>
			<ArcadeMachine />
		</main>
	);
}
