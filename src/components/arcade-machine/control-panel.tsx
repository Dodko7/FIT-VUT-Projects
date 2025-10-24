import { FaChevronDown, FaChevronUp } from "react-icons/fa";
import { Press_Start_2P } from 'next/font/google';

export default function ArcadeControlPanel() {
    return (
        <div
            className="flex w-4/5 h-4/5 bg-gray-800 rounded-lg"
        >
            {/** Arrow Controls */}
            <div
                className="flex flex-col w-1/2 h-full space-y-15 justify-center items-center border-r-2 border-black"
            >
                <FaChevronUp
                    className="text-black text-5xl"
                />
                <FaChevronDown
                    className="text-black text-5xl"
                />
            </div>
            {/** Buttons */}
            <div
                className="grid grid-cols-3 grid-rows-3 w-1/2 h-full place-items-center"
            >
                {/** Y */}
                <button
                    className="bg-yellow-400 w-1/2 h-1/2 rounded-full shadow-lg shadow-yellow-400/50 col-start-2 row-start-1 \
                    text-black items-center justify-center flex text-2xl font-['Press_Start_2P']"
                >
                    Y
                </button>
                {/** X */}
                <button
                    className="bg-blue-800 w-1/2 h-1/2 rounded-full shadow-lg shadow-blue-800/50 col-start-1 row-start-2 \
                    text-black items-center justify-center flex text-2xl font-['Press_Start_2P']"
                >
                    X
                </button>
                {/** A */}
                <button
                    className="bg-green-600 w-1/2 h-1/2 rounded-full shadow-lg shadow-green-600/50 col-start-2 row-start-3 \
                    text-black items-center justify-center flex text-2xl font-['Press_Start_2P']"
                >
                    A
                    </button>
                {/** B */}
                <button
                    className="bg-red-600 w-1/2 h-1/2 rounded-full shadow-lg shadow-red-600/50 col-start-3 row-start-2 \
                    text-black items-center justify-center flex text-2xl font-['Press_Start_2P']"
                >
                    B
                </button>
            </div>
        </div>
    )
}