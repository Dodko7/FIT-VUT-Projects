import Link from "next/link";

/**
 * Props for the menu button.
 */
export type LudoMenuButtonProps = {
	text: string;
	link?: string;
	type?: "submit";
	onClick?: () => void;
};

/**
 * React component representing a ludo menu button.
 */
export default function LudoMenuButton({
	text,
	link,
	onClick,
	type,
}: LudoMenuButtonProps) {
	const cn =
		"ludo-button text-3xl items-center justify-center transition-all duration-200 ease-in-out p-5 w-lg text-center cursor-pointer";

	if (link) {
		return (
			<Link
				href={link}
				className={cn}
			>
				{text}
			</Link>
		);
	} else if (type) {
		return (
			<button
				className={cn}
				type={type}
			>
				{text}
			</button>
		);
	} else {
		return (
			<button
				className={cn}
				onClick={onClick}
			>
				{text}
			</button>
		);
	}
}
