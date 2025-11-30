"use-client";

type ScoreItem = {
	name: string;
	points: number;
};

type Props = {
	scores: ScoreItem[];
};

export default function LeaderboardTable({ scores }: Props) {
	return (
		<div>
			<h2 className="items-center font-['Press_Start_2P'] text-2xl text-yellow-400">
				PACMAN Leaderboard
			</h2>
			<ul>
				{scores.map((score, index) => (
					<li
						key={index}
						className="flex justify-between text-white"
					>
						<span>{score.name}</span>
						<span>{score.points}</span>
					</li>
				))}
			</ul>
		</div>
	);
}
