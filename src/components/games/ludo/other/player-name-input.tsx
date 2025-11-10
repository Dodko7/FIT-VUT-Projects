import { PlayerNameFromColor } from "~/lib/ludo/utils";
import Pawn from "./pawn";

export type PlayerNameInputProps = {
	pawnColor: string;
	inputDivClassName: string;
	isVisible: boolean;
};

export default function PlayerNameInput({
	pawnColor,
	inputDivClassName,
	isVisible
}: PlayerNameInputProps) {
	const player = PlayerNameFromColor(pawnColor);
	const inputId = `ludo-input-${player.toLowerCase().replace(" ", "-")}`;

	return (
		<div 
			className={"flex w-full items-center justify-between " + (isVisible ? "" : "invisible pointer-events-none")}
			aria-hidden={!isVisible}
		>
			{/** Title */}
			<h2 className="ludo-form-text text-5xl">
				{player}
			</h2>
			{/** Input div */}
			<div
				className={`flex items-center justify-start rounded-[40px] px-4 py-2 ${inputDivClassName} flex-grow ml-10 h-20`}
			>
				{/** Pawn svg */}
				<Pawn
					color={pawnColor}
					height={75}
					width={75}
				/>
				{/** Label */}
				<label
					htmlFor={inputId}
					className="mx-5 text-3xl"
				>
					{player} name:
				</label>
				{/** Input */}
				<input
					type="text"
					id={inputId}
					name={inputId}
					className="text-3xl focus:outline-none focus:ring-0"
				/>
			</div>
		</div>
	);
}
