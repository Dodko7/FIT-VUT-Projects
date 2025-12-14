export type PlayerStatusProps = {
    hasContent: boolean;
	playerName: string;
	completedPawns: number;
	className: string;
	border: boolean;
};

export default function PlayerStatus({
    hasContent,
	playerName,
	completedPawns,
	className,
	border,
}: PlayerStatusProps) {
	return (
		<div
			className={`flex w-1/2 items-center justify-between px-4 text-2xl py-3 ${className} ${border ? "border-r-4 border-black" : ""}`}
		>
			{hasContent && <span>{playerName}</span>}
			{hasContent &&<span>{completedPawns}/4 pawns</span>}
		</div>
	);
}
