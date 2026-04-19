/**
 * @brief Main logic for pacman game  
 * @author Róbert Páleš
 */

/**
 * @brief Game entity 
 * Pacman, ghosts, walls, food 
 */
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

	/**
	 * @brief Create a new block instance 
	 * @param image Block image (walls, pacman, ghosts or null for food)
	 * @param x x coordinate
	 * @param y y coordinate
	 * @param width width of the block 
	 * @param height height of the block
	 */
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

	/**
	 * @brief Update the movement direction, check for collisions
	 * @param direction new direction
	 * @param walls walls
	 * @param block size of single block
	 */
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

	/**
	 * @brief Calculate velocity based on current direction
	 * @param block Single block size
	 */
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

	/**
	 * @brief Reset the block to start position
	 */
	reset() {
		this.x = this.startX;
		this.y = this.startY;
	}

	/**
	 * @brief Check if the block is aligned with the grid, allow turning only at intersections
	 * @param blockSize Single block size
	 * @returns true if aligned, otherwise false
	 */
	isAtCenter(blockSize: number): boolean {
        return this.x % blockSize === 0 && this.y % blockSize === 0;
    }

	/**
	 * @brief Opposite direction of the current one, for 180 turns
	 * @returns Opposite direction
	 */
	getOppositeDirection(): string {
        switch (this.direction) {
            case "U": 
				return "D";
            case "D": 
				return "U";
            case "L": 
				return "R";
            case "R": 
				return "L";
            default: 
				return "";
        }
    }
}

/**
 * @brief Check for collisions between 2 blocks 
 * @param a 1st block
 * @param b 2nd block 
 * @returns True if collision is detected
 */
export function collision(a: Block, b: Block) {
	return (
		a.x < b.x + b.width &&
		a.x + a.width > b.x &&
		a.y < b.y + b.height &&
		a.y + a.height > b.y
	);
}

/**
 * @brief Manage game state
 */
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

	/**
	 * @brief Toogle pause when "esc" key or button is pressed
	 */
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

	/**
	 * @brief Init game model with thr given map data  
	 * @param mapData Array of strings representing the level layout
	 */
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

	/**
	 * @brief Update Pacman image based on current direction
	 */
	updatePacmanImage() {
        if (this.pacman.direction === "U") {
			this.pacman.image = this.pacManUpImg;
		} else if (this.pacman.direction === "D") {
			this.pacman.image = this.pacManDownImg;
		} else if (this.pacman.direction === "L") {
			this.pacman.image = this.pacManLeftImg;
		} else if (this.pacman.direction === "R") {
			this.pacman.image = this.pacManRightImg;
		}	
    }

	/**
	 * @brief Parse map layout and create game blocks 
	 * @param newMapData 
	 */
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
		// After loading start 3s countdown  
		this.startCountdown();
	}

	/**
	 * @brief Reset positions after collision with ghost 
	 */
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

	/**
	 * @brief Main logic for movement
	 * Movement, collisions, teleportation, food, win/loss conditions 
	 */
	move() {
		if (this.isPaused || this.isCountingDown) {
			return;
		}

		// ----PACMAN LOGIC----
		this.pacman.x += this.pacman.velocityX;
		this.pacman.y += this.pacman.velocityY;

		// Teleportation to the opposite side
		if (this.pacman.x <= -this.block) {
            this.pacman.x = this.boardWidth;
        } else if (this.pacman.x >= this.boardWidth) {
            this.pacman.x = -this.block;
        }

		// Wall collisions
		for (let wall of this.walls.values()) {
			if (collision(this.pacman, wall)) {
				this.pacman.x -= this.pacman.velocityX;
				this.pacman.y -= this.pacman.velocityY;
				break;
			}
		}

		// Turning logic (aligned)
		if (this.pacman.isAtCenter(this.block)) {
            if (this.pendingDirection) {
                const testPacman = new Block(
                    null, 
                    this.pacman.x, 
                    this.pacman.y, 
                    this.pacman.width, 
                    this.pacman.height
                );
                
                testPacman.direction = this.pendingDirection;
                testPacman.updateVelocity(this.block);

                testPacman.x += testPacman.velocityX;
                testPacman.y += testPacman.velocityY;

                let canTurn = true;
                for (let wall of this.walls.values()) {
                    if (collision(testPacman, wall)) {
                        canTurn = false;
                        break;
                    }
                }

                if (canTurn) {
                    this.pacman.updateDirection(this.pendingDirection, this.walls, this.block);
                    this.pendingDirection = null; 
                    
                    if (this.pacman.direction === "U") this.pacman.image = this.pacManUpImg;
                    else if (this.pacman.direction === "D") this.pacman.image = this.pacManDownImg;
                    else if (this.pacman.direction === "L") this.pacman.image = this.pacManLeftImg;
                    else if (this.pacman.direction === "R") this.pacman.image = this.pacManRightImg;
                }
            }
        }

		// ----GHOSTS LOGIC----
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

		// GhOSTS MOVEMENT
		for (let ghost of this.ghosts.values()) {
			// Hardcoded movement on hard level and row 9 (to prevent loop direction of the ghosts)
			if (ghost.y === this.block * 9 && ghost.direction !== "U" && ghost.direction !== "D") {
				ghost.updateDirection("U", this.walls, this.block);
			}

			ghost.x += ghost.velocityX;
			ghost.y += ghost.velocityY;

			// Teleportation
			if (ghost.x <= -this.block) {
                ghost.x = this.boardWidth;
            } else if (ghost.x >= this.boardWidth) {
                ghost.x = -this.block;
            }

			// Wall collisions
			for (let wall of this.walls.values()) {
				if (collision(ghost, wall)) {
					ghost.x -= ghost.velocityX;
					ghost.y -= ghost.velocityY;
					const newDir = this.directions[Math.floor(Math.random() * 4)];
					ghost.updateDirection(newDir, this.walls, this.block);
				}
			}
		}

		// ----FOOD LOGIC----
		let foodEaten = null;
		for (let food of this.foods.values()) {
			if (collision(this.pacman, food)) {
				// Update score when food is eaten 
				foodEaten = food;
				this.score += 10;
				break;
			}
		}
		// Delete eaten food
		if (foodEaten) {
			this.foods.delete(foodEaten);
		}
		// All food eaten -> win
		if (this.foods.size === 0) {
			this.win = true;
			return;
		}
	}

	/**
	 * @brief Player input for movement 
	 * @param keyCode The key code pressed
	 */ 
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
		
		if (newDir === this.pacman.getOppositeDirection()) {
            this.pacman.updateDirection(newDir, this.walls, this.block);
            this.updatePacmanImage();
            this.pendingDirection = null;
        }
        else {
            this.pendingDirection = newDir;
        }
    }

	/**
	 * @brief Returns current game state
	 * @returns Current game state
	 */
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

	/**
	 * @brief Starts the 3s countdown after the game start or collision
	 */
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
