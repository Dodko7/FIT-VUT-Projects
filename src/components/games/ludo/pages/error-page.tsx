/**
 * This is precisely what it is called.
 */
type LinkWithText = {
    text: string;
    link: string;
}

/**
 * Props for the error page. At least one redirect link or a close handler should be provided.
 */
export type ErrorPageProps = {
    message: string;
    links?: LinkWithText[];
    onClose?: () => void;
}

/**
 * Ludo error page component. TODO make it something creative.
 * THIS IS A PLACEHOLDER.
 * @param param0 Props for the error page.
 */
export default function LudoErrorPage({ message, links, onClose }: ErrorPageProps) {
    return (
        <div
            className="fixed inset-0 flex flex-col items-center justify-center \
            bg-gradient-to-b from-[#840abd] via-[#0077ff] to-[#00ffcc] text-white w-screen h-screen p-10 gap-10"
        >
            <h1 className="text-3xl font-bold">{message}</h1>
            <div className="flex flex-col gap-2">
                {links?.map(({ text, link }) => (
                    <a key={link} href={link} className="text-lg underline">
                        {text}
                    </a>
                ))}
                {onClose && (
                    <button
                        onClick={onClose}
                        className="text-lg underline"
                    >
                        Close
                    </button>
                )}
            </div>
        </div>
    )
}