"use client";
import { forwardRef, useEffect, useRef, useState, useImperativeHandle } from "react";
import { GameController } from "./pacmanControl";
import { GameModel } from "./pacmanModel";

export class GameView {
	context: CanvasRenderingContext2D;
	canvas: HTMLCanvasElement;

	constructor(canvas: HTMLCanvasElement) {
		this.canvas = canvas;
		this.context = canvas.getContext("2d")!;
	}

	draw(model: GameModel) {
		this.context.clearRect(0, 0, this.canvas.width, this.canvas.height);

		this.context.drawImage(
			model.pacman.image!,
			model.pacman.x,
			model.pacman.y,
			model.pacman.width,
			model.pacman.height,
		);

		for (let ghost of model.ghosts.values()) {
			this.context.drawImage(
				ghost.image!,
				ghost.x,
				ghost.y,
				ghost.width,
				ghost.height,
			);
		}

		for (let wall of model.walls.values()) {
			this.context.drawImage(
				wall.image!,
				wall.x,
				wall.y,
				wall.width,
				wall.height,
			);
		}

		this.context.fillStyle = "white";
		for (let food of model.foods.values()) {
			this.context.fillRect(food.x, food.y, food.width, food.height);
		}

		if (model.isCountingDown) {
			const countdown = this.context;
			countdown.save();
			countdown.fillStyle = "yellow";
			countdown.font = "60px 'Press Start 2P'";
			countdown.textAlign = "center";
			countdown.fillText(
				model.countdownValue > 0 ? model.countdownValue.toString() : "",
				this.canvas.width / 2,
				this.canvas.height / 2,
			);
			countdown.restore();
		}

		if (model.isPaused) {
			const ctx = this.context;
			ctx.save();
			ctx.fillStyle = "yellow";
			ctx.font = "48px 'Press Start 2P'";
			ctx.textAlign = "center";
			ctx.fillText("PAUSED", this.canvas.width / 2, this.canvas.height / 2);
			ctx.restore();
		}
	}
}

const PacmanGame = forwardRef((props, ref) => {
	const canvasRef = useRef<HTMLCanvasElement>(null);
	const controllerRef = useRef<GameController | null>(null);
	
	const [gameState, setGameState] = useState({
		score: 0,
		lives: 3,
		gameOver: false,
		win: false,
		isCountingDown: false,
		isRestarting: false,
		countdownValue: 3,
	});
	const [resetTrigger, setResetTrigger] = useState(0);

	useImperativeHandle(ref, () => controllerRef.current);

	useEffect(() => {
		const canvas = canvasRef.current!;
		const controller = new GameController(canvas, setGameState);
		controllerRef.current = controller;

		controller.init();
		const interval = setInterval(() => controller.update(), 50);

		const movePacman = (e: KeyboardEvent) => {
			if (controller.model.gameOver || controller.model.win) {
				controller.model.isRestarting = true;
				controller.model.gameOver = false;
				controller.model.win = false;
				controller.model.score = 0;
				controller.model.lives = 3;
				controller.model.loadMap();
				controller.model.startCountdown();
				setResetTrigger((prev) => prev + 1);
				return;
			}
			controller.handleKeyPress(e.code);
		};

		document.addEventListener("keyup", movePacman);
		return () => {
			clearInterval(interval);
			document.removeEventListener("keyup", movePacman);
		};
	}, [resetTrigger]);

	return (
		<div className="relative flex min-h-screen items-center justify-center bg-black">
			<canvas
				ref={canvasRef}
				className="border-2 border-[#0f1aa6]"
			/>

			{/*Game statistics*/}
			<div className="absolute top-4 left-1/2 -translate-x-1/2 transform font-['Press_Start_2P'] text-xl text-white">
				❤️ x{gameState.lives} | Score: {gameState.score}
			</div>

			{/*Blinking text game over*/}
			{!gameState.isCountingDown &&
				!gameState.isRestarting &&
				gameState.gameOver && (
					<div className="absolute inset-0 z-[99] flex flex-col items-center justify-center">
						<p className="blink font-['Press_Start_2P'] text-6xl text-red-400 drop-shadow-[0_0_6px_black]">
							GAME OVER
						</p>
						<p className="mt-4 font-['Press_Start_2P'] text-2xl text-yellow-400">
							Score: {gameState.score}
						</p>
						<p className="mt-2 font-['Press_Start_2P'] text-xl text-yellow-400">
							Press any key to restart
						</p>
					</div>
				)}

			{/*Blinking text win*/}
			{!gameState.isCountingDown &&
				!gameState.isRestarting &&
				gameState.win && (
					<div className="absolute inset-0 z-[99] flex flex-col items-center justify-center">
						<p className="blink font-['Press_Start_2P'] text-4xl text-yellow-400 drop-shadow-[0_0_6px_black]">
							YOU WIN!
						</p>
						<p className="mt-4 font-['Press_Start_2P'] text-2xl text-yellow-400">
							Score: {gameState.score}
						</p>
						<p className="mt-2 font-['Press_Start_2P'] text-xl text-yellow-400">
							Press any key to restart
						</p>
					</div>
				)}
		</div>
	);
});

PacmanGame.displayName = "PacmanGame";
export default PacmanGame;