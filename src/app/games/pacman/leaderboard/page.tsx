import LeaderboardTable from "~/components/games/pacman/leaderboard/leaderboard";
import ControlButtons from "~/components/games/pacman/buttons/controlButtons";

export default function PacmanLeaderboardPage() {
    return (
        <div className="relative min-h-screen bg-black">
            <div className="absolute top-10 left-1/20 z-10 flex -translate-x-1/2 transform gap-4">
                <ControlButtons 
                    text="Back"
                    link="/games/pacman/menu"
                />
            </div>

            <div className="flex items-center justify-center min-h-screen pt-20">
                <LeaderboardTable />
            </div>
        </div>
    )
}