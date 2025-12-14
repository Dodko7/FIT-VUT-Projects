/**
 * @brief pacman controls page
 */

import ControlButtons from "~/components/games/pacman/buttons/controlButtons";

/**
 * @brief control panel  
 */
export default function PacmanControlsPage() {
    return (
        <div className="relative min-h-screen bg-black flex items-center justify-center">
            <div className="absolute top-10 left-1/20 z-10 flex -translate-x-1/2 transform">
                <ControlButtons 
                    text="Back"
                    link="/games/pacman/menu"
                />
            </div>
            <div className="font-['Press_Start_2P'] text-yellow-400 rounded-lg border-2 border-blue-900 bg-gray-900 p-10">
                <h1 className="mb-10 text-2xl text-center">
					PACMAN Controls
				</h1>
                <div className="grid grid-rows-2 grid-cols-2 gap-5 max-w-lg items-center justify-center">
                    <div className="grid grid-cols-3 grid-rows-2 gap-3 select-none text-yellow-400 text-2xl font-bold">
                        <div></div>
                        <div className="flex items-center justify-center w-14 h-14 rounded bg-gray-700">W</div>
                        <div></div>    
                        <div className="flex items-center justify-center w-14 h-14 rounded bg-gray-700">A</div>
                        <div className="flex items-center justify-center w-14 h-14 rounded bg-gray-700">S</div>
                        <div className="flex items-center justify-center w-14 h-14 rounded bg-gray-700">D</div>
                    </div>
                    <div className="grid grid-cols-3 grid-rows-2 gap-3 select-none text-yellow-400 text-2xl font-bold font-['arial']">
                        <div></div>
                        <div className="flex items-center justify-center w-14 h-14 rounded bg-gray-700">↑</div>
                        <div></div>
                        <div className="flex items-center justify-center w-14 h-14 rounded bg-gray-700">←</div>
                        <div className="flex items-center justify-center w-14 h-14 rounded bg-gray-700">↓</div>
                        <div className="flex items-center justify-center w-14 h-14 rounded bg-gray-700">→</div>
                    </div>    
                    <div className="col-span-2 flex flex-row items-center justify-center gap-5 mt-4 select-none text-yellow-400">
                        <div className="flex items-center justify-center w-24 h-14 rounded bg-gray-700 text-2xl font-bold">esc</div>
                        <p className="text-l">Pause / Resume</p>
                    </div>
                </div>
            </div>
        </div>
    )
}