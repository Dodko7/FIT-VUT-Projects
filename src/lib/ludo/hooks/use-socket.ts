import { useEffect, useState } from "react";
import { io, Socket } from "socket.io-client";
import { useQueryClient } from "@tanstack/react-query";

/**
 * Custom hook to manage Socket.io connection for a Ludo game.
 * @param gameId The ID of the game to connect to.
 * @returns An object containing the socket instance and any connection error.
 */
export default function useSocket(gameId: string) {
	const [socket, setSocket] = useState<Socket | null>(null);
	const [connectionError, setConnectionError] = useState<string | null>(null);
	const queryClient = useQueryClient();

	useEffect(() => {
		const socketInstance = io();

		socketInstance.on("connect", () => {
			console.log("Connected to socket");
			socketInstance.emit("join-game", gameId);
		});

		socketInstance.on("connect_error", (err) => {
			console.error("Socket connection error:", err);
			setConnectionError("Failed to connect to game server.");
		});

		socketInstance.on("game-updated", () => {
			console.log("Game updated event received");
			queryClient.invalidateQueries({
				queryKey: ["ludo", "game", Number(gameId)],
			});
		});

		setSocket(socketInstance);

		return () => {
			socketInstance.disconnect();
		};
	}, [gameId, queryClient]);

	return { socket, connectionError };
}
