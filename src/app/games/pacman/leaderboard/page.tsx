import LeaderboardTable from "~/components/games/pacman/leaderboard/leaderboard"

async function getScores() 
{
    return [
        {name: "jano", points: 500},
        {name: "jano", points: 500},
    ];
} 

export default async function PacmanLeaderboard() 
{
    const scores = await getScores();

    return (
        <div className="flex justify-center items-center min-h-screen bg-black">
            <LeaderboardTable scores={scores} />
        </div>
    )
}