import Image from "next/image";
import LudoMenuButton from "~/components/games/ludo/buttons/menu-button";

/**
 * Main menu page for the ludo game.
 */
export default function LudoMenuPage() {
    // Button props
    const buttonProps = [
        { text: "New Game", link: "/games/ludo/setup" },
        { text: "Load Game", link: "/games/ludo/load" },
        { text: "Load from JSON", link: "/games/ludo/load-json" },
        { text: "Back to Arcade", link: "/" },
    ]

    return (
        <div
            className="flex flex-col items-center justify-start w-screen h-screen \
            bg-gradient-to-b from-[#840abd] via-[#0077ff] to-[#00ffcc] py-10 gap-10"
        >
            {/** Title and pawn svg */}
            <div
                className="w-full items-center justify-center flex gap-4 mt-10"
            >
                <h1
                    className="font-['Luckiest_Guy'] text-5xl text-[#ffc916]"
                >
                    Ludo Hero
                </h1>
                <Image
                    src="/ludo/pawn.svg"
                    alt="Ludo Pawn"
                    width={64}
                    height={64}
                />
            </div>
            {/** Menu */}
            <div
                className="flex flex-col flex-grow justify-between items-center w-full h-full my-15"
            >
                {buttonProps.map(({ text, link }) => (
                    <LudoMenuButton key={link} text={text} link={link} />
                ))}
            </div>
        </div>
    )
}