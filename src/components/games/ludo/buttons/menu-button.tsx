import Link from "next/link";

/**
 * Props for the menu button.
 */
export type LudoMenuButtonProps = {
    text: string;
    link: string;
}

/**
 * React component representing a ludo menu button.
 */
export default function LudoMenuButton({ text, link }: LudoMenuButtonProps) {
    return (
        <Link
            href={link}
            className="font-['Russo_One'] text-2xl text-[#2ddada] bg-[#841889] rounded-[15px] \
                    items-center justify-center hover:shadow-lg hover:bg-[#ff00ff] \
                    transition-all duration-200 ease-in-out p-5 w-lg text-center"
        >
            {text}
        </Link>
    )
}