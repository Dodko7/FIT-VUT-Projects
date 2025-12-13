import type { PausedPageProps } from "~/lib/ludo/types";
import LudoMenuButton from "../buttons/menu-button";

/**
 * Ludo pause page component.
 */
export default function LudoPausePage({
	onResume,
	onQuit,
}: PausedPageProps) {
	const buttonProps = [
		{ text: "Resume", onClick: onResume },
		{ text: "Leave", onClick: onQuit },
	];

	return (
		<div className="flex h-screen w-screen flex-col items-center justify-start gap-10 py-10 ludo-bg">
			{/** Title */}
			<div className="mt-10 flex w-full items-center justify-center gap-4">
				<h1 className="ludo-text-primary text-7xl text-[#ffc916]">
					Game Paused
				</h1>
			</div>
			{/** Menu */}
			<div className="my-15 flex h-full w-full flex-grow flex-col items-center justify-start gap-15">
				{buttonProps.map(({ text, onClick }) => (
					<LudoMenuButton
						key={text}
						text={text}
						onClick={onClick}
					/>
				))}
			</div>
		</div>
	);
}
