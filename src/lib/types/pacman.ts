/**
 * @brief Represents a game level
 * @author Róbert Páleš
 */
export type Level = {
    id: number;
    name: string;
    mapData: string[];
};

/**
 * @brief Data payload for submitting a new high score
 */
export type ScoreSubmission = {
    levelId: number;
    playerName: string;
    score: number;
};

/**
 * @brief Represents a single entry in the leaderboard
 */
export type ScoreItem = {
    id: number;
    playerName: string;
    score: number;
    levelId: number;
};