import { createServer } from "node:http";
import { parse } from "node:url";
import next from "next";
import { InitSocketServer } from "./src/lib/ludo/socket.ts"; // TODO

const hostname = "localhost";
const port = 3000;

// Local NextJS
const app = next({ dev: true, hostname, port });
const handle = app.getRequestHandler();

// Node server
// TODO remove debug logs
app.prepare().then(() => {
	const httpServer = createServer((req, res) => {
		try {
			const parsedUrl = parse(req.url!, true);
			handle(req, res, parsedUrl);
		} catch (err) {
			console.error("Error occurred handling", req.url, err);
			res.statusCode = 500;
			res.end("internal server error");
		}
	});

    // Init Socket.io
	const io = InitSocketServer(httpServer);

	io.on("connection", (socket) => {
		console.log("Client connected:", socket.id);

        // Connect to game
		socket.on("join-game", (gameId: string) => {
			const roomName = `game-${gameId}`;
			socket.join(roomName);
			console.log(`Socket ${socket.id} joined room ${roomName}`);
		});

        // Handle game actions
		socket.on("game-action", (gameId: string) => {
			const roomName = `game-${gameId}`;
			io.to(roomName).emit("game-updated");
			console.log(`Game action in ${roomName}, broadcasting update`);
		});

        // Anddd this
		socket.on("disconnect", () => {
			console.log("Client disconnected:", socket.id);
		});
	});

    // Exit on error, listen on port otherwise
	httpServer
		.once("error", (err) => {
			console.error(err);
			process.exit(1);
		})
		.listen(port, () => {
			console.log(`> Ready on http://${hostname}:${port}`);
		});
});
