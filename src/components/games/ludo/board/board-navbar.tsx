import { FaPause } from "react-icons/fa";

export type BoardNavbarProps = {
    gameName: string;
    onPause: () => void;
};

export default function BoardNavbar({ gameName, onPause }: BoardNavbarProps) {
    return (
        <div
            className="ludo-board-navbar flex items-center justify-between px-10 py-4 w-full"
        >
            {/** Game name */}
            <h1 className="text-3xl">
                {gameName}
            </h1>
            {/** Pause button */}
            <FaPause className="text-2xl cursor-pointer" onClick={onPause} />
        </div>
    )
}