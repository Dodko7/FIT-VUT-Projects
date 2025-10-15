import { FaChevronDown, FaChevronUp } from "react-icons/fa";

export default function ArcadeControlPanel() {
    return (
        <div
            className="flex w-4/5 h-4/5 bg-gray-800 rounded-lg"
        >
            {/** Arrow Controls */}
            <div
                className="flex flex-col w-1/3 h-full space-y-15 justify-center items-center border-r-2 border-black"
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
                className="grid grid-cols-3 grid-rows-3 w-2/3 h-full place-items-center"
            >
                {/** Y */}
                <button
                    className="bg-yellow-400 w-1/2 h-1/2 rounded-full shadow-lg shadow-yellow-400/50 col-start-2 row-start-1"
                ></button>
                {/** X */}
                <button
                    className="bg-blue-800 w-1/2 h-1/2 rounded-full shadow-lg shadow-blue-800/50 col-start-1 row-start-2"
                ></button>
                {/** A */}
                <button
                    className="bg-green-600 w-1/2 h-1/2 rounded-full shadow-lg shadow-green-600/50 col-start-2 row-start-3"
                ></button>
                {/** B */}
                <button
                    className="bg-red-600 w-1/2 h-1/2 rounded-full shadow-lg shadow-red-600/50 col-start-3 row-start-2"
                ></button>
            </div>
        </div>
    )
}