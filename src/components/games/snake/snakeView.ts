/**
 * Snake Game View - Vizualizácia
 * 
 * MVC Pattern - View Layer
 * Zodpovedá za:
 * - Rendering hada, jedla, gridu na canvas
 * - Vykreslenie HUD (skóre, lives)
 * - Vizuálne efekty
 * - Rozlíšenie medzi hernými typmi
 * 
 * @author Igor Lacko
 * @date 2025-11-29
 */

import type { GameState, Position } from "./snakeModel";
import type { SnakeGameType } from "~/lib/types/snake";

export interface ViewConfig {
	cellSize: number;
	colors: {
		background: string;
		grid: string;
		snake: string;
		snakeHead: string;
		food: string;
		wall: string;
		text: string;
	};
}

/**
 * Snake Game View
 */
export class SnakeGameView {
	private canvas: HTMLCanvasElement;
	private ctx: CanvasRenderingContext2D;
	private config: ViewConfig;
	private gameType: SnakeGameType;
	
	// Cache pre HUD - avoid flickering
	private lastScore: number = -1;
	private lastLevel: number = -1;
	private lastPlayerName: string = "";

	constructor(
		canvas: HTMLCanvasElement,
		gameType: SnakeGameType,
		cellSize = 25
	) {
		this.canvas = canvas;
		this.gameType = gameType;
		
		const ctx = canvas.getContext("2d");
		if (!ctx) {
			throw new Error("Canvas 2D context not available");
		}
		this.ctx = ctx;

		// Konfigurácia farieb podľa game type
		this.config = {
			cellSize,
			colors: this.getColorScheme(gameType),
		};
	}

	/**
	 * Farebná schéma podľa typu hry
	 */
	private getColorScheme(gameType: SnakeGameType): ViewConfig["colors"] {
		switch (gameType) {
			case "CLASSIC":
				return {
					background: "#000000",
					grid: "#1a1a1a",
					snake: "#00ff00",
					snakeHead: "#00cc00",
					food: "#ff0000",
					wall: "#ffffff",
					text: "#ffffff",
				};
			case "BOX":
				return {
					background: "#0a0a2e",
					grid: "#16213e",
					snake: "#00d9ff",
					snakeHead: "#0099ff",
					food: "#ff6b6b",
					wall: "#ffd700",
					text: "#ffffff",
				};
			case "CAMPAIGN":
				return {
					background: "#1a0033",
					grid: "#2d0052",
					snake: "#ff00ff",
					snakeHead: "#cc00cc",
					food: "#ffff00",
					wall: "#ffffff",
					text: "#ffffff",
				};
		}
	}

	/**
	 * Nastaví veľkosť canvasu
	 */
	public setCanvasSize(gridSize: number): void {
		const canvasSize = gridSize * this.config.cellSize;
		this.canvas.width = canvasSize;
		this.canvas.height = canvasSize;
	}

	/**
	 * Hlavná render funkcia
	 */
	public render(state: GameState, playerName: string, level: number): void {
		this.clearCanvas();
		this.drawGrid(state.gridSize);
		
		// Steny len pre BOX mode
		if (this.gameType === "BOX") {
			this.drawWalls(state.gridSize);
		}

		this.drawFood(state.food);
		this.drawSnake(state.snake);
		
		// HUD len ak sa niečo zmenilo (avoid flickering)
		if (
			state.score !== this.lastScore ||
			level !== this.lastLevel ||
			playerName !== this.lastPlayerName
		) {
			this.clearHUDArea(); // Vymažeme len HUD area
			this.drawHUD(state.score, playerName, level);
			this.lastScore = state.score;
			this.lastLevel = level;
			this.lastPlayerName = playerName;
		}

		// Game Over / Paused overlay
		if (state.isGameOver) {
			this.drawGameOverOverlay(state.score);
		} else if (state.isPaused) {
			this.drawPausedOverlay();
		}
	}

	/**
	 * Vyčistí canvas (len hernú oblasť, zachová HUD area)
	 */
	private clearCanvas(): void {
		this.ctx.fillStyle = this.config.colors.background;
		this.ctx.fillRect(0, 0, this.canvas.width, this.canvas.height);
	}
	
	/**
	 * Vyčistí iba HUD oblasť (horné 35px a dolných 25px)
	 */
	private clearHUDArea(): void {
		this.ctx.fillStyle = this.config.colors.background;
		// Horná časť (skóre + level)
		this.ctx.fillRect(0, 0, this.canvas.width, 35);
		// Dolná časť (meno hráča)
		this.ctx.fillRect(0, this.canvas.height - 25, this.canvas.width, 25);
	}

	/**
	 * Nakreslí mriežku
	 */
	private drawGrid(gridSize: number): void {
		this.ctx.strokeStyle = this.config.colors.grid;
		this.ctx.lineWidth = 1;

		for (let i = 0; i <= gridSize; i++) {
			const pos = i * this.config.cellSize;
			
			// Vertikálne čiary
			this.ctx.beginPath();
			this.ctx.moveTo(pos, 0);
			this.ctx.lineTo(pos, this.canvas.height);
			this.ctx.stroke();

			// Horizontálne čiary
			this.ctx.beginPath();
			this.ctx.moveTo(0, pos);
			this.ctx.lineTo(this.canvas.width, pos);
			this.ctx.stroke();
		}
	}

	/**
	 * Nakreslí steny (BOX mode)
	 */
	private drawWalls(gridSize: number): void {
		this.ctx.strokeStyle = this.config.colors.wall;
		this.ctx.lineWidth = 4;
		this.ctx.strokeRect(2, 2, this.canvas.width - 4, this.canvas.height - 4);
	}

	/**
	 * Nakreslí hada
	 */
	private drawSnake(snake: Position[]): void {
		snake.forEach((segment, index) => {
			const isHead = index === 0;
			this.ctx.fillStyle = isHead
				? this.config.colors.snakeHead
				: this.config.colors.snake;

			const x = segment.x * this.config.cellSize;
			const y = segment.y * this.config.cellSize;
			const size = this.config.cellSize - 2; // Malý gap

			// Zaoblené rohy
			this.roundRect(x + 1, y + 1, size, size, 4);

			// Oči pre hlavu
			if (isHead) {
				this.drawEyes(segment);
			}
		});
	}

	/**
	 * Nakreslí oči hada
	 */
	private drawEyes(headPos: Position): void {
		const x = headPos.x * this.config.cellSize;
		const y = headPos.y * this.config.cellSize;
		const eyeSize = 3;
		const eyeOffset = this.config.cellSize / 4;

		this.ctx.fillStyle = "#000000";
		
		// Ľavé oko
		this.ctx.beginPath();
		this.ctx.arc(
			x + eyeOffset,
			y + eyeOffset,
			eyeSize,
			0,
			Math.PI * 2
		);
		this.ctx.fill();

		// Pravé oko
		this.ctx.beginPath();
		this.ctx.arc(
			x + this.config.cellSize - eyeOffset,
			y + eyeOffset,
			eyeSize,
			0,
			Math.PI * 2
		);
		this.ctx.fill();
	}

	/**
	 * Nakreslí jedlo
	 */
	private drawFood(food: Position): void {
		const x = food.x * this.config.cellSize;
		const y = food.y * this.config.cellSize;
		const centerX = x + this.config.cellSize / 2;
		const centerY = y + this.config.cellSize / 2;
		const radius = this.config.cellSize / 3;

		// Kruh pre jedlo
		this.ctx.fillStyle = this.config.colors.food;
		this.ctx.beginPath();
		this.ctx.arc(centerX, centerY, radius, 0, Math.PI * 2);
		this.ctx.fill();

		// Blikajúci efekt (outline)
		this.ctx.strokeStyle = this.config.colors.food;
		this.ctx.lineWidth = 2;
		this.ctx.stroke();
	}

	/**
	 * Helper pre zaoblené rohy
	 */
	private roundRect(
		x: number,
		y: number,
		width: number,
		height: number,
		radius: number
	): void {
		this.ctx.beginPath();
		this.ctx.moveTo(x + radius, y);
		this.ctx.lineTo(x + width - radius, y);
		this.ctx.quadraticCurveTo(x + width, y, x + width, y + radius);
		this.ctx.lineTo(x + width, y + height - radius);
		this.ctx.quadraticCurveTo(
			x + width,
			y + height,
			x + width - radius,
			y + height
		);
		this.ctx.lineTo(x + radius, y + height);
		this.ctx.quadraticCurveTo(x, y + height, x, y + height - radius);
		this.ctx.lineTo(x, y + radius);
		this.ctx.quadraticCurveTo(x, y, x + radius, y);
		this.ctx.closePath();
		this.ctx.fill();
	}

	/**
	 * Nakreslí HUD (skóre, meno, level)
	 */
	private drawHUD(score: number, playerName: string, level: number): void {
		this.ctx.fillStyle = this.config.colors.text;
		this.ctx.font = "bold 16px 'Press Start 2P', monospace";
		this.ctx.textAlign = "left";

		// Skóre (ľavý horný roh)
		this.ctx.fillText(`SCORE: ${score}`, 10, 25);

		// Level (pravý horný roh)
		this.ctx.textAlign = "right";
		this.ctx.fillText(`LVL ${level}`, this.canvas.width - 10, 25);

		// Meno hráča (stred dole)
		this.ctx.textAlign = "center";
		this.ctx.font = "12px 'Press Start 2P', monospace";
		this.ctx.fillText(playerName, this.canvas.width / 2, this.canvas.height - 10);
	}

	/**
	 * Game Over overlay
	 */
	private drawGameOverOverlay(score: number): void {
		// Tmavý overlay
		this.ctx.fillStyle = "rgba(0, 0, 0, 0.8)";
		this.ctx.fillRect(0, 0, this.canvas.width, this.canvas.height);

		// Text
		this.ctx.fillStyle = "#ff0000";
		this.ctx.font = "bold 32px 'Press Start 2P', monospace";
		this.ctx.textAlign = "center";
		this.ctx.fillText("GAME OVER", this.canvas.width / 2, this.canvas.height / 2 - 40);

		this.ctx.fillStyle = "#ffffff";
		this.ctx.font = "20px 'Press Start 2P', monospace";
		this.ctx.fillText(
			`Final Score: ${score}`,
			this.canvas.width / 2,
			this.canvas.height / 2 + 20
		);

		this.ctx.font = "14px 'Press Start 2P', monospace";
		this.ctx.fillText(
			"Press ENTER to continue",
			this.canvas.width / 2,
			this.canvas.height / 2 + 60
		);
	}

	/**
	 * Paused overlay
	 */
	private drawPausedOverlay(): void {
		// Tmavý overlay
		this.ctx.fillStyle = "rgba(0, 0, 0, 0.6)";
		this.ctx.fillRect(0, 0, this.canvas.width, this.canvas.height);

		// Text
		this.ctx.fillStyle = "#ffff00";
		this.ctx.font = "bold 32px 'Press Start 2P', monospace";
		this.ctx.textAlign = "center";
		this.ctx.fillText("PAUSED", this.canvas.width / 2, this.canvas.height / 2);

		this.ctx.fillStyle = "#ffffff";
		this.ctx.font = "14px 'Press Start 2P', monospace";
		this.ctx.fillText(
			"Press SPACE to resume",
			this.canvas.width / 2,
			this.canvas.height / 2 + 40
		);
	}

	/**
	 * Animovaný efekt pre intro (voliteľné)
	 */
	public drawCountdown(count: number): void {
		this.clearCanvas();
		this.ctx.fillStyle = "#ffffff";
		this.ctx.font = "bold 64px 'Press Start 2P', monospace";
		this.ctx.textAlign = "center";
		this.ctx.fillText(
			count.toString(),
			this.canvas.width / 2,
			this.canvas.height / 2
		);
	}
}
