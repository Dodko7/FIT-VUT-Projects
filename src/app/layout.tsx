import "~/styles/globals.css";

// Fonts
import { Press_Start_2P } from "next/font/google";

const pressStart2P = Press_Start_2P({ subsets: ["latin"], weight: "400" });

export default function RootLayout({
	children,
}: Readonly<{ children: React.ReactNode }>) {
	return (
		<html lang="en">
			<body>{children}</body>
		</html>
	);
}
