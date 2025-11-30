import PacmanMenuButtons from "~/components/games/pacman/buttons/pacmanMenuButtons"

export default function PacmanMenuPage() 
{
    return (
            <div
                className="flex flex-col items-center justify-start w-screen h-screen \
                bg-black py-10 gap-10"
            >
                {/** Menu */}
                <div
                    className="flex flex-col flex-grow justify-center items-center w-full h-full my-15"
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