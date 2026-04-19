/**
 * Snake Game Layout - Provider wrapper
 * 
 * Obaľuje všetky Snake routes v SnakeGameProvider
 * Umožňuje zdieľanie stavu medzi stránkami (gameType, level, currentGameId)
 * 
 * @author Jozef Ondrejicka
 */

import { SnakeGameProvider } from "~/contexts/SnakeGameContext";

export default function SnakeLayout({
	children,
}: {
	children: React.ReactNode;
}) {
	return <SnakeGameProvider>{children}</SnakeGameProvider>;
}
