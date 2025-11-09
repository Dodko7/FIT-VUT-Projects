
export type PlayerNameInputProps = {
	playerIndex: number;
	pawnClassName: string;
	inputDivClassName: string;
};

export default function PlayerNameInput({
	playerIndex,
	pawnClassName,
	inputDivClassName,
}: PlayerNameInputProps) {
	return (
		<div className="flex w-full items-center justify-between">
			{/** Title */}
			<h2 className="ludo-form-text text-5xl">
				Player {playerIndex + 1}
			</h2>
			{/** Input div */}
			<div
				className={`flex items-center justify-start rounded-[15px] px-4 py-2 ${inputDivClassName}`}
			>
				{/** Pawn svg */}
			</div>
		</div>
	);
}
