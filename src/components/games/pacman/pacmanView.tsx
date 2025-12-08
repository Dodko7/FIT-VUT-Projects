"use client";
import {
	forwardRef,
	useEffect,
	useRef,
	useState,
	useImperativeHandle,
} from "react";
import { GameController } from "./pacmanControl";
import { GameModel } from "./pacmanModel";
import {
	getLevels,
	submitLeaderboardScore,
	type Level,
} from "~/lib/pacman/requests";

interface PacmanGameProps {
	onGameStart?: () => void;
}

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
			ctx.fillText(
				"PAUSED",
				this.canvas.width / 2,
				this.canvas.height / 2,
			);
			ctx.restore();
		}
	}
}

const PacmanGame = forwardRef((props: PacmanGameProps, ref) => {
	const canvasRef = useRef<HTMLCanvasElement>(null);
	const controllerRef = useRef<GameController | null>(null);

	const [levels, setLevels] = useState<Level[]>([]);
	const [selectedLevel, setSelectedLevel] = useState<Level | null>(null);
	const [isLoading, setIsLoading] = useState(true);

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

	const [playerName, setPlayerName] = useState("");
	const [isSubmitting, setIsSubmitting] = useState(false);
	const [scoreSaved, setScoreSaved] = useState(false);

	const submitScore = async () => {
		if (!playerName.trim() || !selectedLevel) return;
		setIsSubmitting(true);
		try {
			await submitLeaderboardScore({
				levelId: selectedLevel.id,
				playerName: playerName,
				score: gameState.score,
			});
			setScoreSaved(true);
		} catch (e) {
			console.error("Failed to save score", e);
		} finally {
			setIsSubmitting(false);
		}
	};

	useImperativeHandle(ref, () => controllerRef.current);

	useEffect(() => {
		const fetchLevels = async () => {
			try {
				const data = await getLevels();
				setLevels(data);
			} catch (error) {
				console.error("Failed to load levels", error);
			} finally {
				setIsLoading(false);
			}
		};
		fetchLevels();
	}, []);

	useEffect(() => {
		if (!selectedLevel || !canvasRef.current) return;
		const mapDataToLoad = selectedLevel.map || selectedLevel.mapData || [];

		const canvas = canvasRef.current!;
		const controller = new GameController(
			canvas,
			setGameState,
			mapDataToLoad,
		);
		controllerRef.current = controller;

		controller.init();
		const interval = setInterval(() => controller.update(), 50);

		const movePacman = (e: KeyboardEvent) => {
			if ((controller.model.gameOver || controller.model.win) &&!scoreSaved) {
				return;
			}

			if (controller.model.gameOver || controller.model.win) {
				setScoreSaved(false);
				setPlayerName("");

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
	}, [selectedLevel, resetTrigger, scoreSaved]);

	return (
		<div className="relative flex min-h-screen items-center justify-center bg-black">
			{!selectedLevel && (
				<div className="z-10 flex min-h-[500px] max-w-lg flex-col items-center justify-center rounded-lg border-2 border-blue-900 bg-gray-900 p-4">
					<h1 className="mb-10 font-['Press_Start_2P'] text-3xl text-yellow-400">
						SELECT LEVEL
					</h1>

					{isLoading ?
						<p className="font-['Press_Start_2P'] text-yellow-400">
							Loading levels...
						</p>
					:	<div className="flex min-w-[300px] flex-col gap-10">
							{levels.map((level) => (
								<button
									key={level.id}
									onClick={() => {
										setSelectedLevel(level);
										if (props.onGameStart) {
											props.onGameStart();
										}
									}}
									className="\ cursor-pointer font-['Press_Start_2P'] text-xl text-yellow-400 transition-all duration-200 ease-in-out hover:text-amber-200 hover:underline"
								>
									{level.name}
								</button>
							))}
							{levels.length === 0 && (
								<p className="font-['Press_Start_2P'] text-sm text-red-400">
									No levels found
								</p>
							)}
						</div>
					}
				</div>
			)}
			{selectedLevel && (
				<>
					<canvas
						ref={canvasRef}
						className="border-2 border-[#0f1aa6]"
					/>

					<div className="absolute top-4 left-1/2 -translate-x-1/2 transform font-['Press_Start_2P'] text-xl text-white">
						❤️ x{gameState.lives} | Score: {gameState.score}
					</div>

					{!gameState.isCountingDown &&
						!gameState.isRestarting &&
						(gameState.gameOver || gameState.win) && (
							<div className="absolute inset-0 z-[99] flex flex-col items-center justify-center bg-black/70">
								<p
									className="blink font-['Press_Start_2P'] text-4xl text-yellow-400 drop-shadow-[0_0_6px_black]"
									style={{
										color:
											gameState.win ? "#facc15" : (
												"#f87171"
											),
									}}
								>
									{gameState.win ? "YOU WIN!" : "GAME OVER"}
								</p>
								<p className="mt-4 font-['Press_Start_2P'] text-2xl text-yellow-400">
									Score: {gameState.score}
								</p>
								{!scoreSaved && (
									<div className="flex flex-col items-center gap-4">
										<p className="mb-2 font-['Press_Start_2P'] text-xs text-yellow-400">
											Enter your name to save score
										</p>
										<input
											autoFocus
											maxLength={15}
											value={playerName}
											onChange={(e) =>
												setPlayerName(e.target.value)
											}
											className="w-85 border-2 border-yellow-400 p-3 text-center font-['Press_Start_2P'] text-xl text-yellow-400"
											placeholder="Player"
										/>
										<button
											onClick={submitScore}
											disabled={
												isSubmitting || !playerName
											}
											className="mt-2 cursor-pointer bg-yellow-400 px-6 py-3 font-['Press_Start_2P'] text-white transition-colors disabled:cursor-not-allowed"
										>
											{isSubmitting ?
												"Saving..."
											:	"Save score"}
										</button>
									</div>
								)}
							</div>
						)}
				</>
			)}
		</div>
	);
});

PacmanGame.displayName = "PacmanGame";
export default PacmanGame;
