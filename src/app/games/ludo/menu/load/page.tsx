"use client";

import { useQuery, useQueryClient } from "@tanstack/react-query";
import { useRouter } from "next/navigation";
import LudoMenuButton from "~/components/games/ludo/buttons/menu-button";
import LudoMenuHeader from "~/components/games/ludo/other/menu-header";
import LudoSavedGame from "~/components/games/ludo/other/saved-game";
import LudoErrorPage from "~/components/games/ludo/pages/error-page";
import LudoLoadingPage from "~/components/games/ludo/pages/loading-page";
import { DeleteGame } from "~/lib/ludo/client-api/delete";
import { LoadAllGames, LoadGameById } from "~/lib/ludo/client-api/load";
import { type MenuGame, type Result } from "~/lib/ludo/types";

export default function LudoLoadPage() {
	const queryClient = useQueryClient();
    const { data: games, isLoading, error } = useQuery<MenuGame[]>({
		queryKey: ["ludo-saved-games"],
		queryFn: LoadAllGames
	});

	const router = useRouter();

	if (error) {
		return <LudoErrorPage message={error?.message || "Unknown error"} />;
	}

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
				{(games || []).map((game) => (
					<LudoSavedGame
						key={game.id}
						game={game}
						onPlay={async (gameId) => {
							const res: Result = await LoadGameById(gameId);
							if (res.success) {
								router.push("/games/ludo/gameplay");
							}
						}}
						onDelete={async (gameId) => {
							await DeleteGame(gameId.toString());
							queryClient.invalidateQueries({ queryKey: ["ludo-saved-games"] });
						}}
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
