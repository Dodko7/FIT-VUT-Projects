import type { CircularButtonProps } from "../buttons/circular-button";
import CircularButton from "../buttons/circular-button";
import type { ColorPickerProps } from "~/lib/ludo/types";

/**
 * Basic color picker with the app's circular buttons.
 * @param param0 Current color and change handler.
 */
export default function ColorPicker({
	currentColor,
	onColorChange,
}: ColorPickerProps) {
	const colorButtons: CircularButtonProps[] = [
		{
			text: "",
			className: "ludo-red-button",
			onClick: () => onColorChange("RED"),
			isActive: currentColor === "RED",
		},
		{
			text: "",
			className: "ludo-yellow-button",
			onClick: () => onColorChange("YELLOW"),
			isActive: currentColor === "YELLOW",
		},
		{
			text: "",
			className: "ludo-green-button",
			onClick: () => onColorChange("GREEN"),
			isActive: currentColor === "GREEN",
		},
		{
			text: "",
			className: "ludo-blue-button",
			onClick: () => onColorChange("BLUE"),
			isActive: currentColor === "BLUE",
		},
	];

	return (
		<div className="flex w-full items-center justify-between">
			<h2 className="ludo-form-text text-5xl">Your Color:</h2>
			{colorButtons.map((props, index) => (
				<CircularButton
					key={index}
					{...props}
				/>
			))}
		</div>
	);
}
