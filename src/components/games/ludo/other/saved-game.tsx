"use client";

import { FaPlay, FaTrash } from "react-icons/fa";
import type { MenuGame } from "~/lib/ludo/types"

type SavedGameProps = {
    game: MenuGame;
    onPlay: (gameId: number) => void;
    onDelete: (gameId: number) => void;
};

/**
 * A field showing a saved Ludo game in the load menu.
 * @param param0 The game object, and play/delete handlers.
 */
export default function LudoSavedGame({ game, onPlay, onDelete }: SavedGameProps) {
    // Formatted player and bot counts
    const playerMessage = game.players === 1 ? "1 player" : `${game.players} players`;
    const botMessage = game.bots === 0 ? "" : (
        game.bots === 1 ? ", 1 bot" : `, ${game.bots} bots`
    );

    return (
        <div
            className="flex justify-between items-center w-full px-15 py-5 rounded-[40px] ludo-secondary"
        >
            {/** Game name */}
            <h2 className="text-2xl font-bold">
                {game.name}
            </h2>
            {/** X Players, Y Bots */}
            <p className="text-2xl flex text-start w-md">
                {playerMessage}{botMessage}
            </p>
            {/** Last played date */}
            <p className="text-2xl w-md text-start">
                Last played: {new Date(game.lastPlayed).toLocaleDateString()}
            </p>
            {/** Play and trash buttons */}
            <div
                className="flex justify-start gap-7 items-center"
            >
                <FaPlay
                    className="text-4xl cursor-pointer ludo-icon"
                    onClick={() => onPlay(game.id)}
                />
                <FaTrash
                    className="text-4xl cursor-pointer ludo-icon"
                    onClick={() => onDelete(game.id)}
                />
            </div>
        </div>
    )
}