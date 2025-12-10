/**
 * @brief pacman menu page
 */

import PacmanMenuButtons from "~/components/games/pacman/buttons/pacmanMenuButtons"

/**
 * @brief display pacman menu 
 * @returns pacman menu buttons
 */
export default function PacmanMenuPage() 
{
    return (
            <div
                className="flex flex-col items-center justify-center w-screen h-screen \
                bg-black py-10 gap-10"
            >
                <div
                    className="max-w-md flex flex-col justify-center items-center min-h-[300px] bg-gray-900 rounded-lg border-2 border-blue-900 p-4"
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
                        text = "Controls"
                        link = "/games/pacman/controls"
                    />
                    <PacmanMenuButtons
                        text = "Quit"
                        link = "/"
                    />
                </div>
            </div>
        )   
} 