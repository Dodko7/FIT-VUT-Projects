/**
 * @brief Pacman game controller
 * Connects game model and view
 */

import { GameModel } from "./pacmanModel";
import { GameView } from "./pacmanView";

export class GameController {
	model: GameModel;
	view: GameView;
	onStateChange: (state: any) => void;

	/**
	 * @brief Init the game controller
	 * @param canvas html canvas element for rendering
	 * @param onStateChange callback to update state 
	 * @param mapData array representing level layout 
	 */
	constructor(
		canvas: HTMLCanvasElement,
		onStateChange: (state: any) => void,
		mapData: string[]
	) {
		this.model = new GameModel(mapData);
		this.view = new GameView(canvas);
		this.onStateChange = onStateChange;

		canvas.height = this.model.boardHeight;
		canvas.width = this.model.boardWidth;
	}

	/**
	 * @brief Setup initial game state
	 * Load map, start countdown, init ghosts directions  
	 */
	init() {
		this.model.loadMap();
		this.model.startCountdown();
		
		for (let ghost of this.model.ghosts.values()) {
			const newDir = this.model.directions[Math.floor(Math.random() * 4)];
			ghost.updateDirection(newDir, this.model.walls, this.model.block);
		}
	}

	/**
	 * @brief Main game loop
	 * Drawing, game movement  
	 */
	update() {
		this.view.draw(this.model);

		this.onStateChange(this.model.getState());

		if (this.model.isCountingDown || this.model.gameOver || this.model.win) {
			return;
		}

		this.model.move();
		this.onStateChange(this.model.getState());
	}

	/**
	 * @brief Handle keyboard input  
	 * @param keyCode The keyboard code as string  
	 */
	handleKeyPress(keyCode: string) {
	// Trigger pause/resume when "esc" is pressed 	
    if (keyCode === "Escape") {
        this.model.togglePause();
        return;
    }

    if (this.model.isPaused) {
		return;
	}

	// Model function for pacman movement   
    this.model.movePacman(keyCode);
}
}
