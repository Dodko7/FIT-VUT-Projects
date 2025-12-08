import PacmanMenuButtons from "~/components/games/pacman/buttons/pacmanMenuButtons"

export default function PacmanMenuPage() 
{
    return (
            <div
                className="flex flex-col items-center justify-center w-screen h-screen \
                bg-black py-10 gap-10"
            >
                <div
                    className="max-w-lg flex flex-col justify-center items-center min-h-[300px] bg-gray-900 rounded-lg border-2 border-blue-900 p-4"
                >
                    <PacmanMenuButtons
                        text = "Start"
                        link = "/games/pacman/gameplay"
                    />
                    <PacmanMenuButtons
                        text = "Leaderboard"
                        link = "/games/pacman/leaderboard"
                    />
                    <PacmanMenuButtons
                        text = "Quit"
                        link = "/"
                    />
                </div>
            </div>
        )
} 