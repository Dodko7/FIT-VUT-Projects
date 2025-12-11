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
	return (
		<div 
			className={"flex w-full items-center justify-between " + (isVisible ? "" : "invisible pointer-events-none")}
			aria-hidden={!isVisible}
		>
			{/** Title */}
			<h2 className="ludo-form-text text-5xl">
				Your name
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
					htmlFor="player-name-input"
					className="mx-5 text-3xl"
				>
					Enter here:
				</label>
				{/** Input */}
				<input
					type="text"
					id="player-name-input"
					name="player-name-input"
					className="text-3xl focus:outline-none focus:ring-0"
				/>
			</div>
		</div>
	);
}
