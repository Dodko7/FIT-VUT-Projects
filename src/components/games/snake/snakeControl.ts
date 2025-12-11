/**
 * Snake Game Controller - Herný Controller
 * 
 * MVC Pattern - Controller Layer
 * Zodpovedá za:
 * - Inicializáciu hry
 * - Game loop
 * - Input handling (keyboard, touch)
 * - Prepojenie Model a View
 * - Auto-save stavu
 * - Komunikáciu s API
 * 
 * @author Igor Lacko
 * @date 2025-11-29
 */

import { SnakeGameModel, type Direction } from "./snakeModel";
import { SnakeGameView } from "./snakeView";
import { getSoundManager } from "./snakeSound";
import type { SnakeGameType } from "~/lib/types/snake";

export interface GameControllerConfig {
	gameId: string;
	gameType: SnakeGameType;
	level: number;
	playerName: string;
	gridSize?: number;
	cellSize?: number;
	autoSaveInterval?: number; // ms
	enableSound?: boolean;
}

export interface GameCallbacks {
	onGameOver?: (score: number) => void;
	onScoreChange?: (score: number) => void;
	onError?: (error: string) => void;
}

/**
 * Snake Game Controller
 */
export class SnakeGameController {
	private model: SnakeGameModel;
	private view: SnakeGameView;
	private config: GameControllerConfig;
	private callbacks: GameCallbacks;
	private soundManager = getSoundManager();

	// Game loop
	private gameLoopInterval: ReturnType<typeof setInterval> | null = null;
	private autoSaveInterval: ReturnType<typeof setInterval> | null = null;
	private animationFrameId: number | null = null;
	private lastUpdateTime: number = 0;
	private lastScore: number = 0;

	// Input handling
	private boundKeyHandler: (e: KeyboardEvent) => void;

	constructor(
		canvas: HTMLCanvasElement,
		config: GameControllerConfig,
		callbacks: GameCallbacks = {}
	) {
		this.config = {
			gridSize: 20,
			cellSize: 25,
			autoSaveInterval: 5000, // 5 sekúnd
			enableSound: true,
			...config,
		};
		this.callbacks = callbacks;

		// Inicializácia Model a View
		this.model = new SnakeGameModel(
			config.gameType,
			config.level,
			this.config.gridSize
		);

		this.view = new SnakeGameView(
			canvas,
			config.gameType,
			this.config.cellSize
		);

		this.view.setCanvasSize(this.config.gridSize!);

		// Bind input handler
		this.boundKeyHandler = this.handleKeyPress.bind(this);
	}

	/**
	 * Načíta uložený stav z API
	 */
	public async loadSavedState(): Promise<boolean> {
		try {
			const response = await fetch(
				`/api/snake/game-state?gameId=${this.config.gameId}`
			);

			if (!response.ok) {
				return false;
			}

			const result = await response.json() as {
				success: boolean;
				data?: {
					snakePositions: string;
					foodPosition: string;
					direction: string;
					speed: number;
				};
			};

			if (result.success && result.data) {
				// Deserializuj stav
				const serializedState = JSON.stringify({
					snake: JSON.parse(result.data.snakePositions),
					food: JSON.parse(result.data.foodPosition),
					direction: result.data.direction,
					speed: result.data.speed,
					score: 0, // Score sa načíta z Game table
				});

				this.model = SnakeGameModel.deserialize(
					serializedState,
					this.config.gameType,
					this.config.level,
					this.config.gridSize
				);

				return true;
			}

			return false;
		} catch (error) {
			console.error("Failed to load saved state:", error);
			return false;
		}
	}

	/**
	 * Spustí hru
	 */
	public start(): void {
		// Pridaj keyboard listener
		window.addEventListener("keydown", this.boundKeyHandler);

		// Začni game loop
		this.startGameLoop();

		// Začni auto-save
		this.startAutoSave();

		// Prvý render
		this.render();
	}

	/**
	 * Zastaví hru a cleanup
	 */
	public stop(): void {
		this.stopGameLoop();
		this.stopAutoSave();
		window.removeEventListener("keydown", this.boundKeyHandler);
		
		// Final render pri game over
		if (this.model.isGameOver()) {
			this.render();
		}
	}

	/**
	 * Začne game loop
	 */
	private startGameLoop(): void {
		if (this.gameLoopInterval || this.animationFrameId) return;

		const speed = this.model.getSpeed();
		this.lastUpdateTime = performance.now();
		
		// Použijeme setInterval pre logic update (podľa speed)
		this.gameLoopInterval = setInterval(() => {
			this.update();
		}, speed);
		
		// requestAnimationFrame pre smooth rendering (60 FPS)
		const renderLoop = (currentTime: number) => {
			// Renderujeme len ak nie je pauza alebo game over
			if (!this.model.isPaused() && !this.model.isGameOver()) {
				this.render();
			}
			
			// Continue loop
			if (this.animationFrameId !== null) {
				this.animationFrameId = requestAnimationFrame(renderLoop);
			}
		};
		
		this.animationFrameId = requestAnimationFrame(renderLoop);
	}

	/**
	 * Zastaví game loop
	 */
	private stopGameLoop(): void {
		if (this.gameLoopInterval) {
			clearInterval(this.gameLoopInterval);
			this.gameLoopInterval = null;
		}
		
		if (this.animationFrameId !== null) {
			cancelAnimationFrame(this.animationFrameId);
			this.animationFrameId = null;
		}
	}

	/**
	 * Hlavný update (každý frame)
	 */
	private update(): void {
		if (this.model.isPaused() || this.model.isGameOver()) {
			return;
		}

		const previousScore = this.lastScore;
		const isAlive = this.model.update();
		const currentScore = this.model.getScore();

		// Detekcia jedenia jedla (score sa zmenil)
		if (currentScore > previousScore) {
			this.lastScore = currentScore;
			if (this.config.enableSound) {
				this.soundManager.playEatSound();
			}
		}

		// Callback pre zmenu score
		if (this.callbacks.onScoreChange) {
			this.callbacks.onScoreChange(currentScore);
		}

		// Game Over handling
		if (!isAlive) {
			if (this.config.enableSound) {
				this.soundManager.playGameOverSound();
			}
			this.handleGameOver();
		}
		
		// Render sa volá v requestAnimationFrame loop, nie tu
	}

	/**
	 * Render frame
	 */
	private render(): void {
		const state = this.model.getState();
		const barrier = this.model.getCampaignBarrier();
		this.view.render(state, this.config.playerName, this.config.level, barrier);
	}

	/**
	 * Handling inputu z klávesnice
	 */
	private handleKeyPress(event: KeyboardEvent): void {
		// Prevencia scrollovania stránky
		if (["ArrowUp", "ArrowDown", "ArrowLeft", "ArrowRight", " "].includes(event.key)) {
			event.preventDefault();
		}

		switch (event.key) {
			case "ArrowUp":
			case "w":
			case "W":
				this.model.setDirection("UP");
				break;

			case "ArrowDown":
			case "s":
			case "S":
				this.model.setDirection("DOWN");
				break;

			case "ArrowLeft":
			case "a":
			case "A":
				this.model.setDirection("LEFT");
				break;

			case "ArrowRight":
			case "d":
			case "D":
				this.model.setDirection("RIGHT");
				break;

			case " ": // Space = pauza
			case "p":
			case "P":
			case "Escape":
				this.togglePause();
				break;

			case "Enter":
				if (this.model.isGameOver()) {
					this.handleGameOverConfirm();
				}
				break;
		}
	}

	/**
	 * Toggle pauzy
	 */
	private togglePause(): void {
		this.model.togglePause();
		this.render();

		if (this.config.enableSound) {
			if (this.model.isPaused()) {
				this.soundManager.playPauseSound();
			} else {
				this.soundManager.playResumeSound();
			}
		}

		if (this.model.isPaused()) {
			this.stopAutoSave();
		} else {
			this.startAutoSave();
		}
	}

	/**
	 * Auto-save každých X sekúnd
	 */
	private startAutoSave(): void {
		if (this.autoSaveInterval) return;

		this.autoSaveInterval = setInterval(() => {
			void this.saveGameState();
		}, this.config.autoSaveInterval);
	}

	/**
	 * Zastaví auto-save
	 */
	private stopAutoSave(): void {
		if (this.autoSaveInterval) {
			clearInterval(this.autoSaveInterval);
			this.autoSaveInterval = null;
		}
	}

	/**
	 * Uloží stav hry do DB
	 */
	private async saveGameState(): Promise<void> {
		if (this.model.isGameOver()) return;

		try {
			const state = this.model.getState();

			const response = await fetch("/api/snake/game-state", {
				method: "POST",
				headers: { "Content-Type": "application/json" },
				body: JSON.stringify({
					gameId: this.config.gameId,
					snakePositions: JSON.stringify(state.snake),
					foodPosition: JSON.stringify(state.food),
					direction: state.direction,
					speed: state.speed,
				}),
			});

			if (!response.ok) {
				console.error("Failed to save game state");
			}

			// Update score v Game table
			await fetch("/api/snake/game", {
				method: "PATCH",
				headers: { "Content-Type": "application/json" },
				body: JSON.stringify({
					gameId: this.config.gameId,
					action: "updateScore",
					score: state.score,
				}),
			});
		} catch (error) {
			console.error("Auto-save failed:", error);
			if (this.callbacks.onError) {
				this.callbacks.onError("Failed to save game");
			}
		}
	}

	/**
	 * Game Over handling
	 */
	private handleGameOver(): void {
		this.stopGameLoop();
		this.stopAutoSave();

		const finalScore = this.model.getScore();

		// Callback
		if (this.callbacks.onGameOver) {
			this.callbacks.onGameOver(finalScore);
		}

		// Uloží finálny stav do DB
		void this.finishGame(finalScore);
	}

	/**
	 * Ukončí hru a uloží výsledok
	 */
	private async finishGame(finalScore: number): Promise<void> {
		try {
			// Update hry na FINISHED
			await fetch("/api/snake/game", {
				method: "PATCH",
				headers: { "Content-Type": "application/json" },
				body: JSON.stringify({
					gameId: this.config.gameId,
					status: "FINISHED",
					score: finalScore,
					playerName: this.config.playerName,
					gameType: this.config.gameType,
				}),
			});

			// Vymaž game state (už nie je potrebný)
			await fetch("/api/snake/game-state", {
				method: "DELETE",
				headers: { "Content-Type": "application/json" },
				body: JSON.stringify({
					gameId: this.config.gameId,
				}),
			});

			console.log("Game finished and saved successfully");
		} catch (error) {
			console.error("Failed to finish game:", error);
		}
	}

	/**
	 * Potvrdenie Game Over (Enter)
	 */
	private handleGameOverConfirm(): void {
		// Tento callback sa použije v React komponente
		// Pre zobrazenie Game Over modalu
		console.log("Game Over confirmed");
	}

	/**
	 * Pauza hry (volané z UI)
	 */
	public pause(): void {
		this.model.setPaused(true);
		this.stopAutoSave();
		this.render();
	}

	/**
	 * Resume hry (volané z UI)
	 */
	public resume(): void {
		this.model.setPaused(false);
		this.startAutoSave();
		this.render();
	}

	/**
	 * Nastaví smer (volané z touch controls)
	 */
	public setDirection(direction: Direction): void {
		this.model.setDirection(direction);
	}

	/**
	 * Getter pre stav
	 */
	public getScore(): number {
		return this.model.getScore();
	}

	public isGameOver(): boolean {
		return this.model.isGameOver();
	}

	public isPaused(): boolean {
		return this.model.isPaused();
	}
}
