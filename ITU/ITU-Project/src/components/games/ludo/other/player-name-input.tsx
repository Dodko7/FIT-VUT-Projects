import Pawn from "./pawn";

export type PlayerNameInputProps = {
	pawnColor: string;
	inputDivClassName: string;
	isVisible: boolean;
	htmlFor: string;
};

export default function PlayerNameInput({
	pawnColor,
	inputDivClassName,
	isVisible,
	htmlFor,
}: PlayerNameInputProps) {
	return (
		<div 
			className={"flex w-full items-center justify-between " + (isVisible ? "" : "invisible pointer-events-none")}
			aria-hidden={!isVisible}
		>
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
					htmlFor={htmlFor}
					className="mx-5 text-3xl"
				>
					Enter here:
				</label>
				{/** Input */}
				<input
					type="text"
					id={htmlFor}
					name={htmlFor}
					className="text-3xl focus:outline-none focus:ring-0"
				/>
			</div>
		</div>
	);
}
