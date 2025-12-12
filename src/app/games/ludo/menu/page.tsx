"use client";

import Image from "next/image";
import { useRef, useState } from "react";
import LudoMenuButton from "~/components/games/ludo/buttons/menu-button";
import { LoadFromJSON } from "~/lib/ludo/client-api/load";
import { useRouter } from "next/navigation";
import LudoErrorPage from "~/components/games/ludo/pages/error-page";
import LudoLoadingPage from "~/components/games/ludo/pages/loading-page";

/**
 * Main menu page for the ludo game.
 */
export default function LudoMenuPage() {

    // Loading and error states
    const [loading, setLoading] = useState(false);
    const [error, setError] = useState<string | null>(null);

    // Router
    const router = useRouter();

    // Button props
    const buttonProps = [
        { text: "New Game", link: "/games/ludo/menu/setup" },
        { text: "Load Game", link: "/games/ludo/menu/load" },
        { text: "Back to Arcade", link: "/" },
    ];

    if (loading) {
        return <LudoLoadingPage />;
    } else if (error) {
        return <LudoErrorPage
            message={error}
            onClose={() => setError(null)}
        />;
    }

    return (
        <div
            className="flex flex-col items-center justify-start w-full h-full py-10 gap-10"
        >
            {/** Title and pawn svg */}
            <div
                className="w-full items-center justify-center flex gap-4 mt-10"
            >
                <h1
                    className="ludo-text-primary text-7xl text-[#ffc916]"
                >
                    Ludo Hero
                </h1>
                <Image
                    src="/ludo/pawn.svg"
                    alt="Ludo Pawn"
                    width={128}
                    height={128}
                />
            </div>
            {/** Menu */}
            <div
                className="flex flex-col flex-grow justify-between items-center w-full h-full my-15"
            >
                {buttonProps.map(({ text, link }, index) => (
                    <LudoMenuButton key={index} text={text} link={link} />
                ))}
            </div>
        </div>
    )
}