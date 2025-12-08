export class Block {
	image: HTMLImageElement | null;
	x: number;
	y: number;
	width: number;
	height: number;
	startX: number;
	startY: number;
	direction: string;
	velocityX: number;
	velocityY: number;

	constructor(
		image: HTMLImageElement | null,
		x: number,
		y: number,
		width: number,
		height: number,
	) {
		this.image = image;
		this.x = x;
		this.y = y;
		this.width = width;
		this.height = height;
		this.startX = x;
		this.startY = y;
		this.direction = "R";
		this.velocityX = 0;
		this.velocityY = 0;
	}

	updateDirection(direction: string, walls: Set<Block>, block: number) {
		const prevDir = this.direction;
		this.direction = direction;
		this.updateVelocity(block);

		this.x += this.velocityX;
		this.y += this.velocityY;

		for (let wall of walls.values()) {
			if (collision(this, wall)) {
				this.x -= this.velocityX;
				this.y -= this.velocityY;
				this.direction = prevDir;
				this.updateVelocity(block);
				return;
			}
		}
	}

	updateVelocity(block: number) {
		if (this.direction == "U") {
			this.velocityX = 0;
			this.velocityY = -block / 4;
		} else if (this.direction == "D") {
			this.velocityX = 0;
			this.velocityY = block / 4;
		} else if (this.direction == "L") {
			this.velocityX = -block / 4;
			this.velocityY = 0;
		} else if (this.direction == "R") {
			this.velocityX = block / 4;
			this.velocityY = 0;
		}
	}

	reset() {
		this.x = this.startX;
		this.y = this.startY;
	}
}

export function collision(a: Block, b: Block) {
	return (
		a.x < b.x + b.width &&
		a.x + a.width > b.x &&
		a.y < b.y + b.height &&
		a.y + a.height > b.y
	);
}

export class GameModel {
	rows: number;
	cols: number;
	block = 32;
	boardWidth: number;
	boardHeight: number;
	walls = new Set<Block>();
	foods = new Set<Block>();
	ghosts = new Set<Block>();
	pacman: Block;
	directions = ["U", "D", "L", "R"];
	score = 0;
	lives = 3;
	gameOver = false;
	win = false;
	isCountingDown = false;
	isRestarting = false;
	isPaused = false;

	togglePause() {
		this.isPaused = !this.isPaused;
	}

	mapData: string[];

	wallImg: HTMLImageElement;
	blueGhostImg: HTMLImageElement;
	orangeGhostImg: HTMLImageElement;
	pinkGhostImg: HTMLImageElement;
	redGhostImg: HTMLImageElement;
	pacManUpImg: HTMLImageElement;
	pacManDownImg: HTMLImageElement;
	pacManLeftImg: HTMLImageElement;
	pacManRightImg: HTMLImageElement;

	constructor(mapData: string[]) {
		this.mapData = mapData;
		this.rows = this.mapData.length;
        this.cols = this.mapData[0].length;
		this.boardWidth = this.cols * this.block;
		this.boardHeight = this.rows * this.block;

		this.wallImg = new Image();
		this.blueGhostImg = new Image();
		this.orangeGhostImg = new Image();
		this.pinkGhostImg = new Image();
		this.redGhostImg = new Image();
		this.pacManUpImg = new Image();
		this.pacManDownImg = new Image();
		this.pacManLeftImg = new Image();
		this.pacManRightImg = new Image();

		this.wallImg.src = "/pacman/wall.png";
		this.blueGhostImg.src = "/pacman/blueGhost.png";
		this.redGhostImg.src = "/pacman/redGhost.png";
		this.orangeGhostImg.src = "/pacman/orangeGhost.png";
		this.pinkGhostImg.src = "/pacman/pinkGhost.png";
		this.pacManUpImg.src = "/pacman/pacmanUp.png";
		this.pacManDownImg.src = "/pacman/pacmanDown.png";
		this.pacManLeftImg.src = "/pacman/pacmanLeft.png";
		this.pacManRightImg.src = "/pacman/pacmanRight.png";
	}

	loadMap(newMapData?: string[]) {
		if (newMapData) {
            this.mapData = newMapData;
            this.rows = this.mapData.length;
            this.cols = this.mapData[0].length;
            this.boardWidth = this.cols * this.block;
            this.boardHeight = this.rows * this.block;
        }

        this.walls.clear();
        this.foods.clear();
        this.ghosts.clear();

		for (let r = 0; r < this.rows; r++) {
			for (let c = 0; c < this.cols; c++) {
				const row = this.mapData[r];
				const mapChar = row[c];

				const x = c * this.block;
				const y = r * this.block;

				if (mapChar == "1") {
					const wall = new Block(
						this.wallImg,
						x,
						y,
						this.block,
						this.block,
					);
					this.walls.add(wall);
				} else if (mapChar == "b") {
					const ghost = new Block(
						this.blueGhostImg,
						x,
						y,
						this.block,
						this.block,
					);
					this.ghosts.add(ghost);
				} else if (mapChar == "r") {
					const ghost = new Block(
						this.redGhostImg,
						x,
						y,
						this.block,
						this.block,
					);
					this.ghosts.add(ghost);
				} else if (mapChar == "p") {
					const ghost = new Block(
						this.pinkGhostImg,
						x,
						y,
						this.block,
						this.block,
					);
					this.ghosts.add(ghost);
				} else if (mapChar == "o") {
					const ghost = new Block(
						this.orangeGhostImg,
						x,
						y,
						this.block,
						this.block,
					);
					this.ghosts.add(ghost);
				} else if (mapChar == "P") {
					this.pacman = new Block(
						this.pacManRightImg,
						x,
						y,
						this.block,
						this.block,
					);
				} else if (mapChar == " ") {
					const food = new Block(null, x + 14, y + 14, 4, 4);
					this.foods.add(food);
				}
			}
		}
		this.startCountdown();
	}

	resetPositions() {
		this.startCountdown();
		this.pacman.reset();
		this.pacman.velocityX = 0;
		this.pacman.velocityY = 0;

		for (let ghost of this.ghosts.values()) {
			ghost.reset();
			const newDir = this.directions[Math.floor(Math.random() * 4)];
			ghost.updateDirection(newDir, this.walls, this.block);
		}
	}

	move() {
		if (this.isPaused || this.isCountingDown) {
			return;
		}

		this.pacman.x += this.pacman.velocityX;
		this.pacman.y += this.pacman.velocityY;

		for (let wall of this.walls.values()) {
			if (collision(this.pacman, wall)) {
				this.pacman.x -= this.pacman.velocityX;
				this.pacman.y -= this.pacman.velocityY;
				break;
			}
		}

		if (this.pendingDirection) {
			const testPacman = new Block(
				this.pacman.image,
				this.pacman.x,
				this.pacman.y,
				this.pacman.width,
				this.pacman.height,
			);

			testPacman.direction = this.pendingDirection;
			testPacman.updateVelocity(this.block);
			testPacman.x += testPacman.velocityX;
			testPacman.y += testPacman.velocityY;

			let canMove = true;
			for (let wall of this.walls.values()) {
				if (collision(testPacman, wall)) {
					canMove = false;
					break;
				}
			}

			if (canMove) {
				this.pacman.x = Math.round(this.pacman.x / 8) * 8;
				this.pacman.y = Math.round(this.pacman.y / 8) * 8;
				this.pacman.updateDirection(
					this.pendingDirection,
					this.walls,
					this.block,
				);
				this.lastDirection = this.pendingDirection;
				this.pendingDirection = null;

				if (this.lastDirection === "U") {
					this.pacman.image = this.pacManUpImg;
				} else if (this.lastDirection === "D") {
					this.pacman.image = this.pacManDownImg;
				} else if (this.lastDirection === "L") {
					this.pacman.image = this.pacManLeftImg;
				} else if (this.lastDirection === "R") {
					this.pacman.image = this.pacManRightImg;
				}
			}
		}

		for (let ghost of this.ghosts.values()) {
			if (collision(ghost, this.pacman)) {
				this.lives -= 1;
				if (this.lives === 0) {
					this.gameOver = true;
					return;
				}
				this.resetPositions();
				return;
			}
		}

		for (let ghost of this.ghosts.values()) {
			if (
				ghost.y === this.block * 9 &&
				ghost.direction !== "U" &&
				ghost.direction !== "D"
			) {
				ghost.updateDirection("U", this.walls, this.block);
			}

			ghost.x += ghost.velocityX;
			ghost.y += ghost.velocityY;

			for (let wall of this.walls.values()) {
				if (
					collision(ghost, wall) ||
					ghost.x <= 0 ||
					ghost.x + ghost.width >= this.boardWidth
				) {
					ghost.x -= ghost.velocityX;
					ghost.y -= ghost.velocityY;
					const newDir =
						this.directions[Math.floor(Math.random() * 4)];
					ghost.updateDirection(newDir, this.walls, this.block);
				}
			}
		}

		let foodEaten = null;
		for (let food of this.foods.values()) {
			if (collision(this.pacman, food)) {
				foodEaten = food;
				this.score += 10;
				break;
			}
		}
		if (foodEaten) {
			this.foods.delete(foodEaten);
		}

		if (this.foods.size === 0) {
			this.win = true;
			return;
		}
	}

	movePacman(keyCode: string) {
		if (this.gameOver || this.win || this.isCountingDown || this.isPaused) {
			return;
		}

		let newDir = null;
		if (keyCode == "ArrowUp" || keyCode == "KeyW") {
			newDir = "U";
		} else if (keyCode == "ArrowDown" || keyCode == "KeyS") {
			newDir = "D";
		} else if (keyCode == "ArrowLeft" || keyCode == "KeyA") {
			newDir = "L";
		} else if (keyCode == "ArrowRight" || keyCode == "KeyD") {
			newDir = "R";
		}

		if (!newDir) {
			return;
		}

		const oldX = this.pacman.x;
		const oldY = this.pacman.y;

		this.pacman.updateDirection(newDir, this.walls, this.block);

		if (oldX !== this.pacman.x || oldY !== this.pacman.y) {
			this.lastDirection = newDir;
			this.pendingDirection = null;

			if (this.lastDirection === "U") {
				this.pacman.image = this.pacManUpImg;
			} else if (this.lastDirection === "D") {
				this.pacman.image = this.pacManDownImg;
			} else if (this.lastDirection === "L") {
				this.pacman.image = this.pacManLeftImg;
			} else if (this.lastDirection === "R") {
				this.pacman.image = this.pacManRightImg;
			}
		} else {
			this.pendingDirection = newDir;
		}
	}

	getState() {
		return {
			score: this.score,
			lives: this.lives,
			gameOver: this.gameOver,
			win: this.win,
			isCountingDown: this.isCountingDown,
			isRestarting: this.isRestarting,
			countdownValue: this.countdownValue,
		};
	}

	startCountdown() {
		this.isCountingDown = true;
		this.countdownValue = 3;

		if (this.countdownTimer) {
			clearInterval(this.countdownTimer);
		}

		this.countdownTimer = window.setInterval(() => {
			this.countdownValue -= 1;

			if (this.countdownValue <= 0) {
				clearInterval(this.countdownTimer!);
				this.isCountingDown = false;
				this.isRestarting = false;
				for (let ghost of this.ghosts.values()) {
					const newDir =
						this.directions[Math.floor(Math.random() * 4)];
					ghost.updateDirection(newDir, this.walls, this.block);
				}
			}
		}, 1000);
	}
}
