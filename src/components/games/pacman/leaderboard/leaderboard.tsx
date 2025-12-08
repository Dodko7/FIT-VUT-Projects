"use client";
import { useState, useEffect } from "react";
import { getLeaderboard, type ScoreItem } from "~/lib/pacman/requests";

type Props = {
    initialScores?: ScoreItem[];
};

const LEVELS = [
    { id: 1, label: "Easy"},
    { id: 2, label: "Medium"},
    { id: 3, label: "Hard"},
];

export default function LeaderboardTable({ initialScores }: Props) {
    const [activeLevel, setActiveLevel] = useState<number>(1);
    const [scores, setScores] = useState<ScoreItem[]>(initialScores || []);

    useEffect(() => {
        const fetchData = async () => {
            try {
                const data = await getLeaderboard(activeLevel);
                setScores(data);
            } catch (error) {
                console.error("Failed to load leaderboard:", error);
            }
        };

        fetchData();
    }, [activeLevel]);

    return (
        <div className="max-w-md mx-auto p-4 bg-gray-900 rounded-lg border-2 border-blue-900">
            <h2 className="text-center font-['Press_Start_2P'] text-2xl text-yellow-400 mb-6">
                PACMAN Leaderboard
            </h2>
            
            <div className="flex justify-center gap-4 mb-6">
                {LEVELS.map((level) => (
                    <button
                        key={level.id}
                        onClick={() => setActiveLevel(level.id)}
                        className={`px-3 py-2 font-['Press_Start_2P'] text-s border-b-4 transition-all  
                            ${activeLevel === level.id 
                                ? `${level} text-yellow-400` 
                                : "text-gray-500 border-transparent hover:text-yellow-400"
                            }`}
                    >
                        {level.label}
                    </button>
                ))}
            </div>

            <ul className="space-y-3 min-h-[500px]">
                {scores.length > 0 ? (
                    scores.map((score, index) => (
                        <li
                            key={score.id || index}
                            className="flex justify-between items-center p-2 border-b border-gray-800 text-white font-mono hover:bg-gray-800 transition"
                        >
                            <span className="flex items-center gap-3">
                                <span className="text-yellow-400 w-8 text-right font-['Press_Start_2P'] text-xs">
                                    #{index + 1}
                                </span>
                                <span className="font-bold text-blue-300 text-s">
                                    {score.playerName}
                                </span>
                            </span>
                            <span className="text-yellow-400 font-['Press_Start_2P'] text-xs">
                                {score.score}
                            </span>
                        </li>
                    ))
                ) : (
                    <li className="text-center text-gray-500 mt-10 font-['Press_Start_2P'] text-xs">
                        NO SCORES... 
                    </li>
                )}
            </ul>
        </div>
    );
}
