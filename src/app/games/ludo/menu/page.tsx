"use client";

import Image from "next/image";
import { useRef, useState } from "react";
import LudoMenuButton from "~/components/games/ludo/buttons/menu-button";
import { LoadFromJSON } from "~/lib/ludo/requests/load";
import { useRouter } from "next/navigation";
import LudoErrorPage from "~/components/games/ludo/pages/error-page";
import LudoLoadingPage from "~/components/games/ludo/pages/loading-page";

/**
 * Main menu page for the ludo game.
 */
export default function LudoMenuPage() {
    // Ref to the current file
    const fileInputRef = useRef<HTMLInputElement>(null);

    // Loading and error states
    const [loading, setLoading] = useState(false);
    const [error, setError] = useState<string | null>(null);

    // Router
    const router = useRouter();

    // Button props
    const buttonProps = [
        { text: "New Game", link: "/games/ludo/menu/setup" },
        { text: "Load Game", link: "/games/ludo/menu/load" },
        { text: "Load from JSON", onClick: () => fileInputRef.current?.click() },
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
                {buttonProps.map(({ text, link, onClick }, index) => (
                    <LudoMenuButton key={index} text={text} link={link} onClick={onClick} />
                ))}
            </div>
            {/** Hidden file dialog */}
            <input
                type="file"
                accept=".json"
                ref={fileInputRef}
                className="hidden"
                onChange={async (e) => {
                    setLoading(true);
                    const res = await LoadFromJSON(e);
                    setLoading(false);
                    if (res.success) {
                        router.push("/games/ludo/gameplay");
                    } else {
                        setError(res.error);
                    }
                }}
            />
        </div>
    )
}