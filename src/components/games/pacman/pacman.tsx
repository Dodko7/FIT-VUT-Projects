"use client";

import { useEffect, useRef, useState } from 'react';

const PacmanGame: React.FC = () => {
  const canvasRef = useRef<HTMLCanvasElement>(null);
  const [gameState, setGameState] = useState({ score: 0, lives: 3, gameOver: false });

  useEffect(() => {
    const canvas = canvasRef.current;

    const rows = 21;
    const cols = 19;
    const block = 32;
    const boardWidth = cols * block;
    const boardHeight = rows * block;
    
    canvas.height = boardHeight;
    canvas.width = boardWidth;
    const context = canvas.getContext('2d');

    const walls = new Set<Block>();
    const foods = new Set<Block>();
    const ghosts = new Set<Block>();
    let pacman: Block;

    const directions = ['U', 'D', 'L', 'R'];
    let score = 0;
    let lives = 3;
    let gameOver = false;

    const Map = [
      "1111111111111111111",
      "1        1        1",
      "1 11 111 1 111 11 1",
      "1                 1",
      "1 11 1 11111 1 11 1",
      "1    1       1    1",
      "1111 1111 1111 1111",
      "OOO1 1       1 1OOO",
      "1111 1 11r11 1 1111",
      "O       bpo       O",
      "1111 1 11111 1 1111",
      "OOO1 1       1 1OOO",
      "1111 1 11111 1 1111",
      "1        1        1",
      "1 11 111 1 111 11 1",
      "1  1     P     1  1",
      "11 1 1 11111 1 1 11",
      "1    1   1   1    1",
      "1 111111 1 111111 1",
      "1                 1",
      "1111111111111111111"
    ];

    class Block {
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

      constructor(image: HTMLImageElement | null, x: number, y: number, width: number, height: number) {
        this.image = image;
        this.x = x;
        this.y = y;
        this.width = width;
        this.height = height;
        this.startX = x;
        this.startY = y;
        this.direction = 'R';
        this.velocityX = 0;
        this.velocityY = 0;
      }

      updateDirection(direction: string) {
        const prevDir = this.direction;
        this.direction = direction;
        this.updateVelocity();

        this.x += this.velocityX;
        this.y += this.velocityY;

        for (let wall of walls.values()) {
          if (collision(this, wall)) {
            this.x -= this.velocityX;
            this.y -= this.velocityY;
            this.direction = prevDir;
            this.updateVelocity();
            return;
          }
        }
      }

      updateVelocity() {
        if (this.direction == 'U') {
          this.velocityX = 0;
          this.velocityY = -block / 4;
        } 
        else if (this.direction == 'D') {
          this.velocityX = 0;
          this.velocityY = block / 4;
        } 
        else if (this.direction == 'L') {
          this.velocityX = -block / 4;
          this.velocityY = 0;
        } 
        else if (this.direction == 'R') {
          this.velocityX = block / 4;
          this.velocityY = 0;
        }
      }

      reset() {
        this.x = this.startX;
        this.y = this.startY;
      }
    }

    const wallImg = new Image();
    const blueGhostImg = new Image();
    const orangeGhostImg = new Image();
    const pinkGhostImg = new Image();
    const redGhostImg = new Image();
    const pacManUpImg = new Image();
    const pacManDownImg = new Image();
    const pacManLeftImg = new Image();
    const pacManRightImg = new Image();

    wallImg.src = "/pacman/wall.png";
    blueGhostImg.src = "/pacman/blueGhost.png";
    redGhostImg.src = "/pacman/redGhost.png";
    orangeGhostImg.src = "/pacman/orangeGhost.png";
    pinkGhostImg.src = "/pacman/pinkGhost.png";
    pacManUpImg.src = "/pacman/pacmanUp.png";
    pacManDownImg.src = "/pacman/pacmanDown.png";
    pacManLeftImg.src = "/pacman/pacmanLeft.png";
    pacManRightImg.src = "/pacman/pacmanRight.png";

    function loadMap() {
      walls.clear();
      foods.clear();
      ghosts.clear();

      for (let r = 0; r < rows; r++) {
        for (let c = 0; c < cols; c++) {
          const row = Map[r];
          const mapChar = row[c];

          const x = c * block;
          const y = r * block;

          if (mapChar == '1') {
            const wall = new Block(wallImg, x, y, block, block);
            walls.add(wall);
          } 
          else if (mapChar == 'b') {
            const ghost = new Block(blueGhostImg, x, y, block, block);
            ghosts.add(ghost);
          } 
          else if (mapChar == 'r') {
            const ghost = new Block(redGhostImg, x, y, block, block);
            ghosts.add(ghost);
          } 
          else if (mapChar == 'p') {
            const ghost = new Block(pinkGhostImg, x, y, block, block);
            ghosts.add(ghost);
          } 
          else if (mapChar == 'o') {
            const ghost = new Block(orangeGhostImg, x, y, block, block);
            ghosts.add(ghost);
          } 
          else if (mapChar == 'P') {
            pacman = new Block(pacManRightImg, x, y, block, block);
          } 
          else if (mapChar == ' ') {
            const food = new Block(null, x + 14, y + 14, 4, 4);
            foods.add(food);
          }
        }
      }
    }

    function resetPositions() {
      pacman.reset();
      pacman.velocityX = 0;
      pacman.velocityY = 0;

      for (let ghost of ghosts.values()) {
        ghost.reset();
        const newDir = directions[Math.floor(Math.random() * 4)];
        ghost.updateDirection(newDir);
      }
    }

    function draw() {
      context.clearRect(0, 0, canvas.width, canvas.height);
      context.drawImage(pacman.image!, pacman.x, pacman.y, pacman.width, pacman.height);

      for (let ghost of ghosts.values()) {
        context.drawImage(ghost.image!, ghost.x, ghost.y, ghost.width, ghost.height);
      }
      for (let wall of walls.values()) {
        context.drawImage(wall.image!, wall.x, wall.y, wall.width, wall.height);
      }

      context.fillStyle = "white";

      for (let food of foods.values()) {
        context.fillRect(food.x, food.y, food.width, food.height);
      }

      context.fillStyle = "white";
      context.font = "16px sans-serif";

      if (gameOver) {
        context.fillText("Game over: " + String(score), block / 2, block / 2);
      } 
      else {
        context.fillText("x" + String(lives) + " " + String(score), block / 2, block / 2);
      }
    }

    function move() {
      pacman.x += pacman.velocityX;
      pacman.y += pacman.velocityY;

      for (let wall of walls.values()) {
        if (collision(pacman, wall)) {
          pacman.x -= pacman.velocityX;
          pacman.y -= pacman.velocityY;

          break;
        }
      }

      for (let ghost of ghosts.values()) {
        if (collision(ghost, pacman)) {
          lives -= 1;

          if (lives == 0) {
            gameOver = true;
            setGameState({ score, lives, gameOver });
            return;
          }

          resetPositions();
        }

        if (ghost.y == block * 9 && ghost.direction != 'U' && ghost.direction != 'D') {
          ghost.updateDirection('U');
        }

        ghost.x += ghost.velocityX;
        ghost.y += ghost.velocityY;
        for (let wall of walls.values()) {
          if (collision(ghost, wall) || ghost.x <= 0 || ghost.x + ghost.width >= boardWidth) {
            ghost.x -= ghost.velocityX;
            ghost.y -= ghost.velocityY;
            const newDir = directions[Math.floor(Math.random() * 4)];
            ghost.updateDirection(newDir);
          }
        }
      }

      let foodEaten = null;

      for (let food of foods.values()) {
        if (collision(pacman, food)) {
          foodEaten = food;
          score += 10;
          break;
        }
      }

      foods.delete(foodEaten);

      if (foods.size == 0) {
        loadMap();
        resetPositions();
      }

      setGameState({ score, lives, gameOver });
    }

    function collision(a: Block, b: Block) {
      return a.x < b.x + b.width &&
        a.x + a.width > b.x &&
        a.y < b.y + b.height &&
        a.y + a.height > b.y;
    }

    function update() {
      if (gameOver) {
        return;
      }
      move();
      draw();
    }

    function movePacman(e: KeyboardEvent) {
      if (gameOver) {
        loadMap();
        resetPositions();

        lives = 3;
        score = 0;
        gameOver = false;

        setGameState({ score, lives, gameOver });
        return;
      }

      if (e.code == "ArrowUp" || e.code == "KeyW") {
        pacman.updateDirection('U');
      } 
      else if (e.code == "ArrowDown" || e.code == "KeyS") {
        pacman.updateDirection('D');
      } 
      else if (e.code == "ArrowLeft" || e.code == "KeyA") {
        pacman.updateDirection('L');
      } 
      else if (e.code == "ArrowRight" || e.code == "KeyD") {
        pacman.updateDirection('R');
      }

      if (pacman.direction == 'U') {
        pacman.image = pacManUpImg;
      }
      else if (pacman.direction == 'D') {
        pacman.image = pacManDownImg;
      }
      if (pacman.direction == 'R') {
        pacman.image = pacManRightImg;
      }
      if (pacman.direction == 'L') {
        pacman.image = pacManLeftImg;
      }
    }

    loadMap();
    
    for (let ghost of ghosts.values()) {
      const newDir = directions[Math.floor(Math.random() * 4)];
      ghost.updateDirection(newDir);
    }

    const interval = setInterval(update, 50);
    document.addEventListener("keyup", movePacman);

    return () => {
      clearInterval(interval);
      document.removeEventListener("keyup", movePacman);
    };
  }, []);

  return (
    <div className="flex items-center justify-center min-h-screen bg-black">
      <canvas ref={canvasRef} className="border-2 border-blue-500" />
    </div>
  );
};

export default PacmanGame;