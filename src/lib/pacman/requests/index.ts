/**
 * @brief API client functions for fetching game data and submitting scores
 * @author Róbert Páleš
 */

import type { Level, ScoreSubmission, ScoreItem} from "~/lib/types/pacman";

/**
 * @brief Fetch all available game levels from the API
 * @returns Promise resolving to an array of Level objects
 * @throws Error if the fetch fails
 */
export const getLevels = async (): Promise<Level[]> => {
    const res = await fetch("/api/pacman/level");
    
    if (!res.ok) {
        throw new Error(`Error fetching levels: ${res.statusText}`);
    }
    
    return await res.json();
};

/**
 * @brief Submits a players score to the db
 * @param data The score submission object containing player name, score, level ID
 * @throws Error if the submission fails
 */
export const submitLeaderboardScore = async (data: ScoreSubmission): Promise<void> => {
    const res = await fetch("/api/pacman/leaderboard", {
        method: "POST",
        headers: {
            "Content-Type": "application/json",
        },
        body: JSON.stringify(data),
    });

    if (!res.ok) {
        throw new Error(`Error saving score: ${res.statusText}`);
    }
};

/**
 * @brief Retrieves the leaderboard scores for a specific level
 * @param levelId ID of the level to filter by
 * @returns Promise resolving to an array of ScoreItem objects (or empty array on error)
 */
export const getLeaderboard = async (levelId: number): Promise<ScoreItem[]> => {
    const res = await fetch(`/api/pacman/leaderboard?levelId=${levelId}`, {
        method: "GET",
        headers: {
            "Content-Type": "application/json",
        },
    });

    if (!res.ok) {
        console.error("Error fetching leaderboard");
        return [];
    }

    return res.json();
};