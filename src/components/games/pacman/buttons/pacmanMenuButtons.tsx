import Link from "next/link";
import React from "react";

type Props = {
	text: string;
	link?: string;
	onClick?: () => void;
};

export default function PacmanMenuButtons({ text, link, onClick }: Props) {
	const c =
		"font-['Press_Start_2P'] text-2xl text-yellow-400 items-center justify-center \
    hover:text-amber-200 hover:underline transition-all duration-200 ease-in-out p-5 w-lg text-center cursor-pointer";

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
