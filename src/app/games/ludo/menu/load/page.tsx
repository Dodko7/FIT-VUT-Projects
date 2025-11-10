"use client";

import { useQuery } from "@tanstack/react-query";
import { useRouter } from "next/navigation";
import LudoMenuButton from "~/components/games/ludo/buttons/menu-button";
import LudoMenuHeader from "~/components/games/ludo/other/menu-header";
import LudoSavedGame from "~/components/games/ludo/other/saved-game";
import LudoErrorPage from "~/components/games/ludo/pages/error-page";
import LudoLoadingPage from "~/components/games/ludo/pages/loading-page";
import { LoadAllGames } from "~/lib/ludo/client-api/load";
import { type MenuGame } from "~/lib/ludo/types";

export default function LudoLoadPage() {
    const { data: games, isLoading, error } = useQuery<MenuGame[]>({
		queryKey: ["ludo-saved-games"],
		queryFn: LoadAllGames
	});

	const router = useRouter();

	if (isLoading) {
		return <LudoLoadingPage />;
	} else if (error || !games) {
		return <LudoErrorPage message={error?.message || "Unknown error"} />;
	}

	// Placeholder games, todo remove
	const placeholderGames = [
			{
				id: 1,
				name: "Sample Game 1",
				lastPlayed: new Date(),
				players: 2,
				bots: 1,
			},

			{
				id: 2,
				lastPlayed: new Date("2024-01-15T10:30:00"),
				name: "Sample Game 2",
				players: 3,
				bots: 0,
			},
		];

	return (
		<div className="flex min-h-full w-full flex-col items-center justify-start overflow-y-auto">
			{/** Header */}
			<LudoMenuHeader
				title="💾 Your Games"
				className="h-1/11"
			/>
			{/** Content */}
			<div
				className="flex w-full flex-grow flex-col items-center justify-start px-25 my-15 gap-10"
			>
				{(placeholderGames).map((game) => (
					<LudoSavedGame
						key={game.id}
						game={game}
						onPlay={(gameId) => console.log("TODO play game", gameId)}
						onDelete={(gameId) => console.log("TODO delete game", gameId)}
					/>
				))}
			</div>
			{/** Go back button */}
			<div
				className="w-full flex items-center justify-center mb-10"
			>
				<LudoMenuButton
					text="Go back"
					link="/games/ludo/menu"
				/>
			</div>
		</div>
	);
}
