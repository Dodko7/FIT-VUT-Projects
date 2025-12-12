import { Server } from "socket.io";

// Global Socket.io server instance
let io: Server | null = null;

/**
 * Initializes the Socket.io server.
 * @param server The HTTP server to attach Socket.io to.
 * @returns The initialized Socket.io server instance.
 */
export function InitSocketServer(server: any) {
	io = new Server(server, {
		cors: {
			origin: "*",
		},
	});

	return io;
}

/**
 * Gets the Socket.io server instance or throws an error if not initialized.
 * @returns The Socket.io server instance.
 */
export function GetSocketServer(): Server {
	if (!io) {
		throw new Error("Socket.io server not initialized.");
	}
	return io;
}
