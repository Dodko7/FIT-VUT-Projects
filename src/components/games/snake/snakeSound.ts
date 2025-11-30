/**
 * Snake Game Sound Manager
 * 
 * @author Igor Lacko
 * @description Web Audio API pre herné zvuky
 */

export class SnakeSoundManager {
	private audioContext: AudioContext | null = null;
	private isMuted: boolean = false;
	private masterVolume: number = 0.3;

	constructor() {
		// Inicializuj AudioContext len v browseri
		if (typeof window !== "undefined") {
			try {
				this.audioContext = new (window.AudioContext || (window as any).webkitAudioContext)();
				
				// Načítaj nastavenie z localStorage
				const savedMute = localStorage.getItem("snakeSoundMuted");
				this.isMuted = savedMute === "true";
			} catch (error) {
				console.warn("Web Audio API not supported:", error);
			}
		}
	}

	/**
	 * Vytvorí oscillator pre zvuky (retro 8-bit štýl)
	 */
	private createOscillator(
		frequency: number,
		duration: number,
		type: OscillatorType = "square"
	): void {
		if (!this.audioContext || this.isMuted) return;

		try {
			const oscillator = this.audioContext.createOscillator();
			const gainNode = this.audioContext.createGain();

			oscillator.type = type;
			oscillator.frequency.setValueAtTime(frequency, this.audioContext.currentTime);

			gainNode.gain.setValueAtTime(this.masterVolume, this.audioContext.currentTime);
			gainNode.gain.exponentialRampToValueAtTime(
				0.01,
				this.audioContext.currentTime + duration
			);

			oscillator.connect(gainNode);
			gainNode.connect(this.audioContext.destination);

			oscillator.start(this.audioContext.currentTime);
			oscillator.stop(this.audioContext.currentTime + duration);
		} catch (error) {
			console.error("Error playing sound:", error);
		}
	}

	/**
	 * Zvuk pre zjedenie jedla
	 */
	public playEatSound(): void {
		if (!this.audioContext || this.isMuted) return;

		// Rýchly "blip" zvuk
		this.createOscillator(800, 0.1, "sine");
		setTimeout(() => {
			this.createOscillator(1200, 0.05, "sine");
		}, 50);
	}

	/**
	 * Zvuk pre game over
	 */
	public playGameOverSound(): void {
		if (!this.audioContext || this.isMuted) return;

		// Zostupná sekvencia (smutný zvuk)
		const notes = [400, 350, 300, 250, 200];
		notes.forEach((freq, index) => {
			setTimeout(() => {
				this.createOscillator(freq, 0.2, "square");
			}, index * 150);
		});
	}

	/**
	 * Zvuk pre pohyb (jemný tick)
	 */
	public playMoveSound(): void {
		if (!this.audioContext || this.isMuted) return;

		// Veľmi krátky a tichý "tick"
		const oscillator = this.audioContext.createOscillator();
		const gainNode = this.audioContext.createGain();

		oscillator.type = "square";
		oscillator.frequency.setValueAtTime(150, this.audioContext.currentTime);

		gainNode.gain.setValueAtTime(0.05, this.audioContext.currentTime);
		gainNode.gain.exponentialRampToValueAtTime(
			0.01,
			this.audioContext.currentTime + 0.02
		);

		oscillator.connect(gainNode);
		gainNode.connect(this.audioContext.destination);

		oscillator.start(this.audioContext.currentTime);
		oscillator.stop(this.audioContext.currentTime + 0.02);
	}

	/**
	 * Zvuk pre kolíziu
	 */
	public playCollisionSound(): void {
		if (!this.audioContext || this.isMuted) return;

		// Drsný "crash" zvuk
		this.createOscillator(100, 0.3, "sawtooth");
	}

	/**
	 * Zvuk pre pauzu
	 */
	public playPauseSound(): void {
		if (!this.audioContext || this.isMuted) return;

		this.createOscillator(600, 0.1, "triangle");
	}

	/**
	 * Zvuk pre resume
	 */
	public playResumeSound(): void {
		if (!this.audioContext || this.isMuted) return;

		this.createOscillator(800, 0.1, "triangle");
	}

	/**
	 * Zvuk pre výber v menu
	 */
	public playSelectSound(): void {
		if (!this.audioContext || this.isMuted) return;

		this.createOscillator(1000, 0.05, "sine");
	}

	/**
	 * Toggle mute
	 */
	public toggleMute(): boolean {
		this.isMuted = !this.isMuted;
		
		// Ulož do localStorage
		if (typeof window !== "undefined") {
			localStorage.setItem("snakeSoundMuted", String(this.isMuted));
		}
		
		return this.isMuted;
	}

	/**
	 * Nastaví hlasitosť (0.0 - 1.0)
	 */
	public setVolume(volume: number): void {
		this.masterVolume = Math.max(0, Math.min(1, volume));
	}

	/**
	 * Gettery
	 */
	public isSoundMuted(): boolean {
		return this.isMuted;
	}

	/**
	 * Cleanup
	 */
	public dispose(): void {
		if (this.audioContext) {
			void this.audioContext.close();
			this.audioContext = null;
		}
	}
}

// Singleton instance
let soundManagerInstance: SnakeSoundManager | null = null;

export function getSoundManager(): SnakeSoundManager {
	if (!soundManagerInstance) {
		soundManagerInstance = new SnakeSoundManager();
	}
	return soundManagerInstance;
}
