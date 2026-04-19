import Link from "next/link";

export type MenuTitleProps = {
    title: string;
    href: string;
}

export default function MenuTitle({ title, href }: MenuTitleProps) {
    return (
        <Link
            href={href}
            className="text-yellow-400 text-2xl font-['Press_Start_2P'] cursor-pointer \
            hover:text-amber-200 hover:underline transition-colors duration-300"
        >
            {title}
        </Link>
    )
}