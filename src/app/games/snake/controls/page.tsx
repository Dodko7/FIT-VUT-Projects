"use client";

import Link from "next/link";

export default function SnakeControlsPage() {
	return (
		<div className="flex min-h-screen flex-col items-center justify-start bg-black text-white pt-16">
			{/* Back Button */}
			<Link
				href="/games/snake"
				className="absolute top-8 left-8 px-6 py-3 bg-gradient-to-b from-[#fef3c7] to-[#fde047] text-black font-['Press_Start_2P'] text-sm rounded-lg hover:from-[#fde047] to-[#facc15] transition-all"
			>
				Back
			</Link>

			{/* Title */}
			<h1 className="mb-20 font-['Press_Start_2P'] text-6xl snake-gradient-text drop-shadow-[0_0_10px_rgba(250,204,21,0.5)]">
				Controls
			</h1>

			{/* Controls Display */}
			<div className="flex flex-col items-center gap-16">
				{/* Up Arrow */}
				<div className="flex items-center justify-center">
					<div className="w-24 h-24 bg-gradient-to-b from-[#fef3c7] via-[#fde047] to-[#fde047] rounded-2xl border-4 border-black shadow-lg flex items-center justify-center">
						<div className="w-0 h-0 border-l-[20px] border-l-transparent border-r-[20px] border-r-transparent border-b-[30px] border-b-black"></div>
						<span className="absolute font-['Press_Start_2P'] text-black text-xl mt-1">w</span>
					</div>
				</div>

				{/* Left, Down, Right Arrows */}
				<div className="flex items-center gap-6">
					{/* Left Arrow */}
					<div className="w-24 h-24 bg-gradient-to-b from-[#fef3c7] via-[#fde047] to-[#fde047] rounded-2xl border-4 border-black shadow-lg flex items-center justify-center">
						<div className="w-0 h-0 border-t-[20px] border-t-transparent border-b-[20px] border-b-transparent border-r-[30px] border-r-black"></div>
						<span className="absolute font-['Press_Start_2P'] text-black text-xl ml-1">a</span>
					</div>

					{/* Down Arrow */}
					<div className="w-24 h-24 bg-gradient-to-b from-[#fef3c7] via-[#fde047] to-[#fde047] rounded-2xl border-4 border-black shadow-lg flex items-center justify-center">
						<div className="w-0 h-0 border-l-[20px] border-l-transparent border-r-[20px] border-r-transparent border-t-[30px] border-t-black"></div>
						<span className="absolute font-['Press_Start_2P'] text-black text-xl mb-1">s</span>
					</div>

					{/* Right Arrow */}
					<div className="w-24 h-24 bg-gradient-to-b from-[#fef3c7] via-[#fde047] to-[#fde047] rounded-2xl border-4 border-black shadow-lg flex items-center justify-center">
						<div className="w-0 h-0 border-t-[20px] border-t-transparent border-b-[20px] border-b-transparent border-l-[30px] border-l-black"></div>
						<span className="absolute font-['Press_Start_2P'] text-black text-xl mr-1">d</span>
					</div>
				</div>

				{/* ESC Key */}
				<div className="flex items-center gap-8 mt-8">
					<div className="w-32 h-24 bg-gradient-to-b from-[#fef3c7] via-[#fde047] to-[#fde047] rounded-2xl border-4 border-black shadow-lg flex items-center justify-center">
						<span className="font-['Press_Start_2P'] text-black text-xl">esc</span>
					</div>
					<span className="font-['Press_Start_2P'] text-3xl snake-gradient-text">
						Pause/Play
					</span>
				</div>
			</div>
		</div>
	);
}
