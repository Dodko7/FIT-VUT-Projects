/**
 * Snake Leaderboard Page
 * 
 * @author Igor Lacko
 * @description Zobrazenie top výsledkov s filtrom podľa typu hry
 */

"use client";

import { useState, useEffect } from "react";
import Link from "next/link";
import type { LeaderboardEntry, SnakeGameType } from "~/lib/types/snake";

const GAME_TYPE_LABELS: Record<SnakeGameType, string> = {
	CLASSIC: "Classic",
	BOX: "Box",
	CAMPAIGN: "Campaign",
};

export default function SnakeLeaderboardPage() {
	const [leaderboard, setLeaderboard] = useState<LeaderboardEntry[]>([]);
	const [selectedType, setSelectedType] = useState<SnakeGameType | "ALL">("ALL");
	const [loading, setLoading] = useState(true);
	const [error, setError] = useState<string | null>(null);

	/**
	 * Načítanie leaderboardu z API
	 */
	const fetchLeaderboard = async (gameType?: SnakeGameType) => {
		setLoading(true);
		setError(null);

		try {
			const url = gameType ?
				`/api/snake/leaderboard?gameType=${gameType}&limit=20`
			:	"/api/snake/leaderboard?limit=20";

			const response = await fetch(url);
			const result = await response.json();

			if (result.success) {
				setLeaderboard(result.data);
			} else {
				setError(result.error || "Failed to load leaderboard");
			}
		} catch (err) {
			console.error("Error fetching leaderboard:", err);
			setError("Failed to load leaderboard");
		} finally {
			setLoading(false);
		}
	};

	// Načítaj leaderboard pri načítaní stránky a pri zmene filtra
	useEffect(() => {
		fetchLeaderboard(selectedType === "ALL" ? undefined : selectedType);
	}, [selectedType]);

	/**
	 * Formátovanie dátumu
	 */
	const formatDate = (date: Date) => {
		return new Date(date).toLocaleDateString("sk-SK", {
			day: "2-digit",
			month: "2-digit",
			year: "numeric",
		});
	};

	return (
		<div className="flex min-h-screen flex-col items-center justify-start bg-black text-white pt-8 pb-16">
			{/* Back Button */}
			<Link
				href="/games/snake"
				className="absolute top-8 left-8 px-6 py-3 bg-gradient-to-b from-[#fef3c7] to-[#fde047] text-black font-['Press_Start_2P'] text-sm rounded-lg hover:from-[#fde047] hover:to-[#facc15] transition-all"
			>
				Back
			</Link>

			{/* Title */}
			<h1 className="mb-8 font-['Press_Start_2P'] text-5xl snake-gradient-text drop-shadow-[0_0_10px_rgba(250,204,21,0.5)]">
				Leaderboard
			</h1>

			{/* Filter Tabs */}
			<div className="flex gap-4 mb-8">
				{(["ALL", "CLASSIC", "BOX", "CAMPAIGN"] as const).map((type) => (
					<button
						key={type}
						onClick={() => setSelectedType(type)}
						className={`px-6 py-2 font-['Press_Start_2P'] text-sm rounded-lg transition-all ${
							selectedType === type
								? "bg-yellow-400 text-black"
								: "bg-gray-800 text-yellow-200 hover:bg-gray-700"
						}`}
					>
						{type === "ALL" ? "All" : GAME_TYPE_LABELS[type]}
					</button>
				))}
			</div>

			{/* Loading State */}
			{loading && (
				<div className="font-['Press_Start_2P'] text-yellow-400">
					Loading...
				</div>
			)}

			{/* Error State */}
			{error && (
				<div className="px-4 py-2 bg-red-900 border-2 border-red-500 rounded text-red-200 font-['Press_Start_2P'] text-xs">
					{error}
				</div>
			)}

			{/* Leaderboard Table */}
			{!loading && !error && leaderboard.length > 0 && (
				<div className="w-[90%] max-w-4xl">
					<div className="bg-gray-900 border-4 border-yellow-600 rounded-lg overflow-hidden">
						{/* Header */}
						<div className="grid grid-cols-6 gap-4 bg-yellow-600 text-black font-['Press_Start_2P'] text-xs p-4">
							<div className="text-center">#</div>
							<div className="col-span-2">Player</div>
							<div className="text-center">Score</div>
							<div className="text-center">Level</div>
							<div className="text-center">Mode</div>
						</div>

						{/* Rows */}
						{leaderboard.map((entry) => (
							<div
								key={entry.gameId}
								className={`grid grid-cols-6 gap-4 font-['Press_Start_2P'] text-xs p-4 border-b border-gray-700 ${
									entry.position <= 3
										? "bg-yellow-900 bg-opacity-20"
										: "hover:bg-gray-800"
								}`}
							>
								<div className="text-center">
									{entry.position === 1 && "🥇"}
									{entry.position === 2 && "🥈"}
									{entry.position === 3 && "🥉"}
									{entry.position > 3 && entry.position}
								</div>
								<div className="col-span-2 text-yellow-200 truncate">
									{entry.playerName}
								</div>
								<div className="text-center text-yellow-400 font-bold">
									{entry.score}
								</div>
								<div className="text-center text-yellow-200">
									{entry.level}
								</div>
								<div className="text-center text-gray-400 text-[10px]">
									{GAME_TYPE_LABELS[entry.gameType]}
								</div>
							</div>
						))}
					</div>
				</div>
			)}

			{/* Empty State */}
			{!loading && !error && leaderboard.length === 0 && (
				<div className="font-['Press_Start_2P'] text-gray-500 text-sm mt-8">
					No scores yet. Be the first to play!
				</div>
			)}

			{/* Refresh Button */}
			<button
				onClick={() => fetchLeaderboard(selectedType === "ALL" ? undefined : selectedType)}
				disabled={loading}
				className="mt-8 px-8 py-3 bg-gradient-to-b from-[#fef3c7] to-[#fde047] text-black font-['Press_Start_2P'] text-sm rounded-lg hover:from-[#fde047] hover:to-[#facc15] transition-all disabled:opacity-50"
			>
				{loading ? "Refreshing..." : "Refresh"}
			</button>
		</div>
	);
}
