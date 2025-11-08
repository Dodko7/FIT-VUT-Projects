import Image from "next/image";
import ArcadeMachine from "~/components/arcade-machine/arcade-machine";

export default function HomePage() {
	return (
		<main className="fixed inset-0 flex flex-col items-center justify-center bg-gradient-to-b from-[#2e026d] to-[#15162c] text-white w-screen h-screen">
			<Image
				className="absolute inset-0 z-0 object-cover w-full h-full"
				src="/arcade-bg.png"
				alt="Background"
				fill
			/>
				<ArcadeMachine />
		</main>
	);
}
