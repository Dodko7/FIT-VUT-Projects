/**
 * Snake Game Layout - Provider wrapper
 * 
 * @author Jozef Ondrejicka
 * @description Obaľuje všetky Snake routes v SnakeGameProvider
 *              pre zdieľanie stavu medzi stránkami
 */

import { SnakeGameProvider } from "~/contexts/SnakeGameContext";

export default function SnakeLayout({
	children,
}: {
	children: React.ReactNode;
}) {
	return <SnakeGameProvider>{children}</SnakeGameProvider>;
}
