import LudoMenuButton from "../buttons/menu-button";
import LudoMenuHeader from "../other/menu-header";
import type { ErrorPageProps } from "~/lib/ludo/types";

/**
 * Ludo error page component.
 * @param param0 Props for the error page.
 */
export default function LudoErrorPage({ message, links, onClose }: ErrorPageProps) {
    return (
        <div className="flex flex-col items-center justify-start w-full h-full py-10 gap-10">
            <LudoMenuHeader title="ERROR" />
            
            <div className="flex flex-col items-center gap-8 w-full max-w-4xl mt-10 px-8">
                <div className="ludo-menu-error rounded-[15px] px-10 py-8 text-3xl flex flex-col items-center gap-4 text-center shadow-lg border-4 border-white/20">
                    <span className="text-5xl">⚠️</span>
                    {message}
                </div>

                <div className="flex flex-col gap-4 mt-8 w-full items-center">
                    {links?.map(({ text, link }) => (
                        <LudoMenuButton
                            key={link}
                            text={text}
                            onClick={() => window.location.href = link}
                        />
                    ))}
                    {onClose && (
                        <LudoMenuButton
                            text="CLOSE"
                            onClick={onClose}
                        />
                    )}
                    {!links && !onClose && (
                         <LudoMenuButton
                            text="BACK TO MENU"
                            link="/games/ludo/menu"
                        />
                    )}
                </div>
            </div>
        </div>
    )
}