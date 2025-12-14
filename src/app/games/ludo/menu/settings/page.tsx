"use client";

import { useState } from "react";
import type { CircularButtonProps } from "~/components/games/ludo/buttons/circular-button";
import CircularButton from "~/components/games/ludo/buttons/circular-button";
import LudoMenuButton from "~/components/games/ludo/buttons/menu-button";
import LudoMenuHeader from "~/components/games/ludo/other/menu-header";

/**
 * Ludo Settings Page. Basically only and option for dice color.
 */
export default function LudoSettingsPage() {
	// Current color
	const [color, setColor] = useState<string>(
		localStorage.getItem("ludoDiceColor") || "white",
	);

	// Click handler
	const saveColor = (newColor: string) => {
		localStorage.setItem("ludoDiceColor", newColor);
		setColor(newColor);
	};

	// Circular button props for picking dice color
	const circularButtons: Partial<CircularButtonProps>[] = [
		{
			className: "ludo-white-button",
			onClick: () => saveColor("white"),
			isActive: color === "white",
		},
		{
			className: "ludo-red-button",
			onClick: () => saveColor("red"),
			isActive: color === "red",
		},
		{
			className: "ludo-green-button",
			onClick: () => saveColor("green"),
			isActive: color === "green",
		},
		{
			className: "ludo-blue-button",
			onClick: () => saveColor("blue"),
			isActive: color === "blue",
		},
		{
			className: "ludo-yellow-button",
			onClick: () => saveColor("yellow"),
			isActive: color === "yellow",
		},
	];
	return (
		<div className="flex min-h-full w-full flex-col items-center justify-start">
			<LudoMenuHeader
				title="⚙️ SETTINGS"
				className="h-1/11"
			/>
			{/** Rest of page */}
			<div className="mt-10 flex h-full w-full flex-col items-start justify-between gap-8">
				<div className="flex w-full flex-col items-start justify-start gap-4">
					<h2 className="ludo-text-primary ml-10 text-5xl">
						Dice Color
					</h2>
					<div className="flex w-full justify-start gap-20 px-10">
						{circularButtons.map((button, index) => (
							<CircularButton
								key={index}
								text={""}
								className={button.className!}
								onClick={button.onClick!}
								isActive={button.isActive!}
							/>
						))}
					</div>
				</div>
				<div
                    className="mb-10 flex w-full items-center justify-center"
                >
					<LudoMenuButton
						text="BACK TO MENU"
						link="/games/ludo/menu"
					/>
				</div>
			</div>
		</div>
	);
}
