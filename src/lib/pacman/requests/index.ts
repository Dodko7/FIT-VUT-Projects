export type Level = {
    id: number;
    name: string;
    mapData: string[];
};

export type ScoreSubmission = {
    levelId: number;
    playerName: string;
    score: number;
};

export type ScoreItem = {
    id: number;
    playerName: string;
    score: number;
    levelId: number;
};

export const getLevels = async (): Promise<Level[]> => {
    const res = await fetch("/api/pacman/level");
    
    if (!res.ok) {
        throw new Error(`Error fetching levels: ${res.statusText}`);
    }
    
    return await res.json();
};

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