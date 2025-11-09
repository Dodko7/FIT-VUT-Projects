import "~/styles/globals.css";

// Fonts
import { Press_Start_2P, Russo_One, Luckiest_Guy } from "next/font/google";

const pressStart2P = Press_Start_2P({
	subsets: ["latin"],
	weight: "400",
	variable: "--font-press-start",
});
const russoOne = Russo_One({
	subsets: ["latin"],
	weight: "400",
	variable: "--font-russo",
});
const luckiestGuy = Luckiest_Guy({
	subsets: ["latin"],
	weight: "400",
	variable: "--font-luckiest-guy",
});

export default function RootLayout({
	children,
}: Readonly<{ children: React.ReactNode }>) {
	return (
		<html lang="en">
			<body
				className={`${pressStart2P.variable} ${russoOne.variable} ${luckiestGuy.variable}`}
			>
				{children}
			</body>
		</html>
	);
}
