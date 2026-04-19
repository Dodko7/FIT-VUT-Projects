/**
 * Snake Game Model - Herná logika
 * 
 * MVC Pattern - Model Layer
 * Zodpovedá za:
 * - Herný stav (pozície hada, jedla, smer)
 * - Hernú logiku (pohyb, kolízie, bodovanie)
 * - Validáciu ťahov
 * - Generovanie jedla
 * 
 * @author Jozef Ondrejicka
 */

import type { SnakeGameType } from "~/lib/types/snake";

export type Direction = "UP" | "DOWN" | "LEFT" | "RIGHT";

export interface Position {
	x: number;
	y: number;
}

export interface GameState {
	snake: Position[]; // [0] je hlava
	food: Position;
	direction: Direction;
	nextDirection: Direction; // Pre buffer inputu
	score: number;
	isGameOver: boolean;
	isPaused: boolean;
	speed: number; // ms medzi frameami
	gridSize: number; // počet buniek (napr. 20x20)
}

/**
 * Snake Game Model
 */
export class SnakeGameModel {
	private state: GameState;
	private gameType: SnakeGameType;
	private level: number;
	private hasWalls: boolean;
	private campaignBarrier: Position[]; // Positions of the "+" barrier for CAMPAIGN mode

	constructor(gameType: SnakeGameType, level: number, gridSize = 20) {
		this.gameType = gameType;
		this.level = level;
		this.hasWalls = gameType === "BOX" || gameType === "CAMPAIGN"; // BOX and CAMPAIGN have walls
		this.campaignBarrier = this.generateCampaignBarrier(gridSize);
		
		// Inicializácia stavu
		this.state = {
			snake: this.initializeSnake(gridSize),
			food: { x: 0, y: 0 }, // Nastavíme v generateFood()
			direction: "RIGHT",
			nextDirection: "RIGHT",
			score: 0,
			isGameOver: false,
			isPaused: false,
			speed: this.calculateSpeed(level),
			gridSize,
		};

		this.generateFood();
	}
	
	/**
	 * Generuje pozície "+" bariéry pre CAMPAIGN mód
	 * Vytvorí kríž v strede hracej plochy (od 6 do 14 na osi)
	 */
	private generateCampaignBarrier(gridSize: number): Position[] {
		if (this.gameType !== "CAMPAIGN") return [];
		
		const barrier: Position[] = [];
		const mid = Math.floor(gridSize / 2);
		
		// Vertical line (from 6 to 14)
		for (let i = 6; i < gridSize - 5; i++) {
			barrier.push({ x: mid, y: i });
		}
		
		// Horizontal line (from 6 to 14)
		for (let i = 6; i < gridSize - 5; i++) {
			barrier.push({ x: i, y: mid });
		}
		
		return barrier;
	}

	/**
	 * Vytvorí počiatočnú pozíciu hada (3 segmenty v ľavom hornom rohu)
	 * Začína na pozícii (3,3) aby sa vyhol bariéram v CAMPAIGN móde
	 */
	private initializeSnake(gridSize: number): Position[] {
		// Start in top-left area to avoid barriers in CAMPAIGN mode
		return [
			{ x: 3, y: 3 }, // Hlava
			{ x: 2, y: 3 },
			{ x: 1, y: 3 },
		];
	}

	/**
	 * Vypočíta rýchlosť podľa levelu (level 1 = pomalý, level 10 = rýchly)
	 * Level 1: 200ms medzi frameami, Level 10: 50ms medzi frameami
	 */
	private calculateSpeed(level: number): number {
		// Level 1: 200ms, Level 10: 50ms
		const baseSpeed = 200;
		const speedReduction = (level - 1) * 15;
		return Math.max(50, baseSpeed - speedReduction);
	}

	/**
	 * Generuje náhodné jedlo na hracej ploche
	 * Kontroluje, aby jedlo nevzniklo na hadovi ani na bariére
	 */
	private generateFood(): void {
		let newFood: Position;
		let attempts = 0;
		const maxAttempts = 100;

		do {
			newFood = {
				x: Math.floor(Math.random() * this.state.gridSize),
				y: Math.floor(Math.random() * this.state.gridSize),
			};
			attempts++;
		} while (
			(this.isPositionOnSnake(newFood) || this.isPositionOnBarrier(newFood)) &&
			attempts < maxAttempts
		);

		this.state.food = newFood;
	}

	/**
	 * Kontrola či je pozícia na hadovi
	 */
	private isPositionOnSnake(pos: Position): boolean {
		return this.state.snake.some(
			(segment) => segment.x === pos.x && segment.y === pos.y
		);
	}
	
	/**
	 * Kontroluje či je zadaná pozícia na CAMPAIGN bariére
	 * Vracia false pre iné herné módy ako CAMPAIGN
	 */
	private isPositionOnBarrier(pos: Position): boolean {
		if (this.gameType !== "CAMPAIGN") return false;
		return this.campaignBarrier.some(
			(barrier) => barrier.x === pos.x && barrier.y === pos.y
		);
	}

	/**
	 * Nastaví nový smer pohybu hada
	 * Validuje, že had nemôže ísť priamo opačným smerom (predídenie samovražde)
	 */
	public setDirection(newDirection: Direction): void {
		if (this.state.isPaused || this.state.isGameOver) return;

		const opposites: Record<Direction, Direction> = {
			UP: "DOWN",
			DOWN: "UP",
			LEFT: "RIGHT",
			RIGHT: "LEFT",
		};

		// Nemôže ísť priamo opačne
		if (opposites[this.state.direction] !== newDirection) {
			this.state.nextDirection = newDirection;
		}
	}

	/**
	 * Hlavná update funkcia - posunie hada o jeden krok
	 * Kontroluje kolízie, jedenie jedla a aktualizuje skóre
	 * @returns true ak hra pokračuje, false ak nastala kolízia (game over)
	 */
	public update(): boolean {
		if (this.state.isPaused || this.state.isGameOver) {
			return !this.state.isGameOver;
		}

		// Aplikuj buffered direction
		this.state.direction = this.state.nextDirection;

		// Vypočítaj novú pozíciu hlavy
		const head = this.state.snake[0]!;
		const newHead = this.getNextPosition(head, this.state.direction);

		// Kontrola kolízií
		if (this.checkCollision(newHead)) {
			this.state.isGameOver = true;
			return false;
		}

		// Pridaj novú hlavu
		this.state.snake.unshift(newHead);

		// Kontrola či zjedol jedlo
		if (newHead.x === this.state.food.x && newHead.y === this.state.food.y) {
			this.state.score += 10 * this.level; // Vyšší level = viac bodov
			this.generateFood();
			// Had rastie (nepridávame pop())
		} else {
			// Normálny pohyb - odstráň chvost
			this.state.snake.pop();
		}

		return true;
	}

	/**
	 * Vypočíta ďalšiu pozíciu hlavy hada na základe smeru
	 * Pre CLASSIC mód implementuje wrap-around (prechod cez okraje)
	 */
	private getNextPosition(current: Position, direction: Direction): Position {
		const next = { ...current };

		switch (direction) {
			case "UP":
				next.y -= 1;
				break;
			case "DOWN":
				next.y += 1;
				break;
			case "LEFT":
				next.x -= 1;
				break;
			case "RIGHT":
				next.x += 1;
				break;
		}

		// Ak nemá steny, wrap around
		if (!this.hasWalls) {
			if (next.x < 0) next.x = this.state.gridSize - 1;
			if (next.x >= this.state.gridSize) next.x = 0;
			if (next.y < 0) next.y = this.state.gridSize - 1;
			if (next.y >= this.state.gridSize) next.y = 0;
		}

		return next;
	}

	/**
	 * Kontroluje všetky typy kolízií
	 * - Kolízia so stenou (BOX a CAMPAIGN mód)
	 * - Kolízia s bariérou (CAMPAIGN mód)
	 * - Kolízia so sebou samým
	 */
	private checkCollision(position: Position): boolean {
		// Kolízia so stenou (len pre BOX a CAMPAIGN mode)
		if (this.hasWalls) {
			if (
				position.x < 0 ||
				position.x >= this.state.gridSize ||
				position.y < 0 ||
				position.y >= this.state.gridSize
			) {
				return true;
			}
		}
		
		// Kolízia s CAMPAIGN barrier
		if (this.gameType === "CAMPAIGN") {
			if (this.campaignBarrier.some(b => b.x === position.x && b.y === position.y)) {
				return true;
			}
		}

		// Kolízia so sebou (okrem hlavy)
		return this.state.snake
			.slice(1)
			.some((segment) => segment.x === position.x && segment.y === position.y);
	}

	/**
	 * Prepína stav pauzy (zapne/vypne)
	 */
	public togglePause(): void {
		this.state.isPaused = !this.state.isPaused;
	}

	public setPaused(paused: boolean): void {
		this.state.isPaused = paused;
	}

	/**
	 * Vracia aktuálny stav hry (read-only kópia)
	 */
	public getState(): Readonly<GameState> {
		return { ...this.state };
	}
	
	public getCampaignBarrier(): ReadonlyArray<Position> {
		return this.campaignBarrier;
	}

	public getScore(): number {
		return this.state.score;
	}

	public isGameOver(): boolean {
		return this.state.isGameOver;
	}

	public isPaused(): boolean {
		return this.state.isPaused;
	}

	public getSpeed(): number {
		return this.state.speed;
	}

	/**
	 * Serializuje aktuálny stav hry do JSON formátu
	 * Používa sa pre uloženie do databázy
	 */
	public serialize(): string {
		return JSON.stringify({
			snake: this.state.snake,
			food: this.state.food,
			direction: this.state.direction,
			score: this.state.score,
			speed: this.state.speed,
		});
	}

	/**
	 * Deserializuje uložený stav hry z JSON formátu
	 * Vytvára nový Model a načíta do neho uložené dáta
	 */
	public static deserialize(
		data: string,
		gameType: SnakeGameType,
		level: number,
		gridSize = 20
	): SnakeGameModel {
		const model = new SnakeGameModel(gameType, level, gridSize);
		
		try {
			const savedState = JSON.parse(data) as {
				snake: Position[];
				food: Position;
				direction: Direction;
				score: number;
				speed: number;
			};

			model.state.snake = savedState.snake;
			model.state.food = savedState.food;
			model.state.direction = savedState.direction;
			model.state.nextDirection = savedState.direction;
			model.state.score = savedState.score;
			model.state.speed = savedState.speed;
		} catch (error) {
			console.error("Failed to deserialize game state:", error);
		}

		return model;
	}

	/**
	 * Resetuje hru do počiatočného stavu
	 * Zachováva úroveň a veľkosť hracej plochy
	 */
	public reset(): void {
		this.state = {
			snake: this.initializeSnake(this.state.gridSize),
			food: this.state.food,
			direction: "RIGHT",
			nextDirection: "RIGHT",
			score: 0,
			isGameOver: false,
			isPaused: false,
			speed: this.calculateSpeed(this.level),
			gridSize: this.state.gridSize,
		};
		this.generateFood();
	}
}
