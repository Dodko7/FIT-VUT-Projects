import Link from "next/link";

/**
 * Props for the menu button.
 */
export type LudoMenuButtonProps = {
	text: string;
	link?: string;
	onClick?: () => void;
};

/**
 * React component representing a ludo menu button.
 */
export default function LudoMenuButton({
	text,
	link,
	onClick,
}: LudoMenuButtonProps) {
	const cn =
		"ludo-button text-3xl items-center justify-center transition-all duration-200 ease-in-out p-5 w-lg text-center";

	return link ?
			<Link
				href={link}
				className={cn}
			>
				{text}
			</Link>
		:	<button
				className={cn}
				onClick={onClick}
			>
				{text}
			</button>;
}
