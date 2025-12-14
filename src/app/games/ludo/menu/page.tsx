"use client";

import Image from "next/image";
import { useRef, useState } from "react";
import LudoMenuButton from "~/components/games/ludo/buttons/menu-button";
import { LoadFromJSON } from "~/lib/ludo/client-api/load";
import { StatusIndicator } from "~/components/games/ludo/other/status-indicator";
import StatusIndicatorState from "~/lib/ludo/enum/status-indicator-state";

/**
 * Main menu page for the ludo game.
 */
export default function LudoMenuPage() {
	// Status indicator
	const [statusState, setStatusState] = useState<StatusIndicatorState | null>(
		null,
	);

	// JSON input
	const fileInputRef = useRef<HTMLInputElement>(null);

	const onClickLoad = () => {
		fileInputRef.current?.click();
	};

	// Status indicator message
	const getMessage = () => {
		switch (statusState) {
			case StatusIndicatorState.LOADING:
				return "Loading...";
			case StatusIndicatorState.SUCCESS:
				return "Successfully loaded!";
			case StatusIndicatorState.ERROR:
				return "Error loading game.";
			default:
				return "";
		}
	};

	// Wrapper to set it for a while
	const setError = () => {
		setStatusState(StatusIndicatorState.ERROR);
		setTimeout(() => setStatusState(null), 3000);
	};

	const setSuccess = () => {
		setStatusState(StatusIndicatorState.SUCCESS);
		setTimeout(() => setStatusState(null), 3000);
	};

	const onFileChange = (e: React.ChangeEvent<HTMLInputElement>) => {
		const file = e.target.files?.[0];
		if (!file) {
			return;
		}

		const reader = new FileReader();
		reader.onload = async (event) => {
			const content = event.target?.result;
			if (typeof content === "string") {
				try {
					const jsonContent = JSON.parse(content);
					const result = await LoadFromJSON(jsonContent);
					if (result.success) {
						setSuccess();
					} else {
						setError();
					}
				} catch (err) {
					setError();
				}
			} else {
				setError();
			}
		};

		reader.readAsText(file);
	};

	// Button props
	const buttonProps = [
		{ text: "New Game", link: "/games/ludo/menu/setup" },
		{ text: "Load Game", link: "/games/ludo/menu/load" },
		{ text: "Load from JSON", onClick: onClickLoad },
		{ text: "Settings", link: "/games/ludo/menu/settings" },
		{ text: "Back to Arcade", link: "/" },
	];

	return (
		<div className="flex h-full w-full flex-col items-center justify-start gap-10 py-10">
			{/** Title and pawn svg */}
			<div className="mt-10 flex w-full items-center justify-center gap-4">
				<h1 className="ludo-text-primary text-7xl text-[#ffc916]">
					Ludo Hero
				</h1>
				<Image
					src="/ludo/pawn.svg"
					alt="Ludo Pawn"
					width={128}
					height={128}
				/>
			</div>
			{/** Menu */}
			<div className="my-15 flex h-full w-full flex-grow flex-col items-center justify-between">
				{buttonProps.map(({ text, link, onClick }, index) => (
					<LudoMenuButton
						key={index}
						text={text}
						link={link}
						onClick={onClick}
					/>
				))}
			</div>
			<input
				type="file"
				ref={fileInputRef}
				onChange={onFileChange}
				accept=".json,application/json"
				style={{ display: "none" }}
			/>
			<StatusIndicator
				isVisible={statusState !== null}
				message={getMessage()}
				type={
					statusState !== null ? statusState : (
						StatusIndicatorState.LOADING
					)
				}
			/>
		</div>
	);
}
