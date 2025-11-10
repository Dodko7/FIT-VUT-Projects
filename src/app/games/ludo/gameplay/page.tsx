"use client";

import { Color } from "@prisma/client";
import { useQuery } from "@tanstack/react-query";
import { useState } from "react";
import BoardNavbar from "~/components/games/ludo/board/board-navbar";
import LudoErrorPage from "~/components/games/ludo/pages/error-page";
import LudoLoadingPage from "~/components/games/ludo/pages/loading-page";
import LudoPausePage from "~/components/games/ludo/pages/pause-page";
import { LoadCurrentGame } from "~/lib/ludo/client-api/load";
import type { FullGame } from "~/lib/ludo/types";

export default function LudoGameplayPage() {
    // Fetch game state
    const { data: game, isLoading, error } = useQuery<FullGame>({
        queryKey: ["ludo", "current-game"],
        queryFn: LoadCurrentGame
    });

    // Is the game paused?
    const [isPaused, setIsPaused] = useState<boolean>(false);

    // Which player
    const [currentTurn, setCurrentTurn] = useState<Color>(Color.RED);
    
    // What did the player roll
    const [diceRoll, setDiceRoll] = useState<number | null>(null);

    // Avaliable pawns to move
    const [avaliablePawns, setAvailablePawns] = useState<number[]>([]);
    
    // Selected pawn
    const [selectedPawnId, setSelectedPawnId] = useState<number | null>(null);
    
    // Available moves for the selected pawn
    const [avaliableMoves, setAvailableMoves] = useState<number[]>([]);

    if (isLoading) {
        return <LudoLoadingPage />;
    } else if (error || !game) {
        return <LudoErrorPage message={error?.message || "Failed to load current game."} />;
    } else if (isPaused) {
        return <LudoPausePage />;
    }

    return (
        <div
            className="w-full h-full flex flex-col items-center justify-center ludo-board-font"
        >
            <BoardNavbar
                gameName={game.name}
                onPause={() => setIsPaused(true)}
            />
            
        </div>
    )
}