import { GameModel } from "./pacmanModel";
import { GameView } from "./pacmanView";

export class GameController {
	model: GameModel;
	view: GameView;
	onStateChange: (state: any) => void;

	constructor(
		canvas: HTMLCanvasElement,
		onStateChange: (state: any) => void,
	) {
		this.model = new GameModel();
		this.view = new GameView(canvas);
		this.onStateChange = onStateChange;

		canvas.height = this.model.boardHeight;
		canvas.width = this.model.boardWidth;
	}

	init() {
		this.model.startCountdown();
		this.model.loadMap();

		for (let ghost of this.model.ghosts.values()) {
			const newDir = this.model.directions[Math.floor(Math.random() * 4)];
			ghost.updateDirection(newDir, this.model.walls, this.model.block);
		}
	}

	update() {
		this.view.draw(this.model);

		this.onStateChange(this.model.getState());

		if (
			this.model.isCountingDown ||
			this.model.gameOver ||
			this.model.win
		) {
			return;
		}

		this.model.move();
		this.onStateChange(this.model.getState());
	}

	handleKeyPress(keyCode: string) {
		this.model.movePacman(keyCode);
	}
}
