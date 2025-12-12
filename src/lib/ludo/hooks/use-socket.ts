import { useEffect, useState } from "react";
import { io, Socket } from "socket.io-client";

/**
 * Custom hook to manage Socket.io connection for a Ludo game.
 * @param gameName The name of the game to connect to.
 * @returns An object containing the socket instance and any connection error.
 */
export default function useSocket(gameName: string) {
	const [socket, setSocket] = useState<Socket | null>(null);
	const [connectionError, setConnectionError] = useState<string | null>(null);

	useEffect(() => {
		const socketInstance = io("http://localhost:3000");

		socketInstance.on("connect", () => {
			console.log("Connected to socket");
			socketInstance.emit("join-game", gameName);
		});

		socketInstance.on("connect_error", (err) => {
			console.error("Socket connection error:", err);
			setConnectionError("Failed to connect to game server.");
		});

		setSocket(socketInstance);

		return () => {
			socketInstance.disconnect();
		};
	}, []);

	return { socket, connectionError };
}
