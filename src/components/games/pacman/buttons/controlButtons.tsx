import Link from "next/link";
import React from "react";

type Props = {
	text: string;
	link?: string;
	onClick?: () => void;
};

export default function ControlButtons({ text, link, onClick }: Props) {
	const c = "cursor-pointer bg-yellow-400 px-5 py-2 font-['Press_Start_2P'] text-white";

	if (link) {
		return (
			<Link
				href={link}
				className={c}
			>
				{text}
			</Link>
		);
	}

	return (
		<button
			className={c}
			onClick={onClick}
		>
			{text}
		</button>
	);
}