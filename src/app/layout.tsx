"use client";

import "~/styles/globals.css";

// Fonts
import { Press_Start_2P, Russo_One, Luckiest_Guy } from "next/font/google";
import { QueryClient, QueryClientProvider } from "@tanstack/react-query";

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

// React query setup
const queryClient = new QueryClient();

export default function RootLayout({
	children,
}: Readonly<{ children: React.ReactNode }>) {
	return (
		<html lang="en">
			<body
				className={`${pressStart2P.variable} ${russoOne.variable} ${luckiestGuy.variable}`}
			>
				<QueryClientProvider client={queryClient}>
					{children}
				</QueryClientProvider>
			</body>
		</html>
	);
}
