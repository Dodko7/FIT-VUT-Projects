import type { PausedPageProps } from "~/lib/ludo/types";
import LudoMenuButton from "../buttons/menu-button";
import StatusIndicatorState from "~/lib/ludo/enum/status-indicator-state";
import { StatusIndicator } from "../other/status-indicator";
import { useState } from "react";

/**
 * Ludo pause page component.
 */
export default function LudoPausePage({
	onResume,
	onQuit,
	onExport,
}: PausedPageProps) {
	const [statusState, setStatusState] = useState<StatusIndicatorState | null>(
		null,
	);

	const onExportWrapper = async () => {
		setStatusState(StatusIndicatorState.LOADING);
		try {
			await onExport();
			setStatusState(StatusIndicatorState.SUCCESS);
		} catch {
			setStatusState(StatusIndicatorState.ERROR);
		}
		setTimeout(() => setStatusState(null), 3000);
	};

	const buttonProps = [
		{ text: "Resume", onClick: onResume },
		{ text: "Leave", onClick: onQuit },
		{ text: "Copy as JSON", onClick: onExportWrapper },
	];

	const getStatusMessage = () => {
		switch (statusState) {
			case StatusIndicatorState.LOADING:
				return "Loading...";
			case StatusIndicatorState.SUCCESS:
				return "Copied to clipboard!";
			case StatusIndicatorState.ERROR:
				return "Error copying to clipboard.";
			default:
				return "";
		}
	};

	return (
		<div className="ludo-bg flex h-screen w-screen flex-col items-center justify-start gap-10 py-10">
			{/** Status indicator for copying JSON to clipboard */}
			<StatusIndicator
				isVisible={statusState !== null}
				type={
					statusState !== null ? statusState : (
						StatusIndicatorState.SUCCESS
					)
				}
				message={getStatusMessage()}
			/>
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
