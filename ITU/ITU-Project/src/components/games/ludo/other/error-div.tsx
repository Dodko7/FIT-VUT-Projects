export type ErrorDivProps = {
    message?: string;
    onClose: () => void;
    isVisible: boolean;
};

export default function ErrorDiv({ message, onClose, isVisible }: ErrorDivProps) {
    return (
        <div
            className={"ludo-menu-error rounded-[15px] px-10 py-4 text-2xl flex justify-between" + (isVisible ? "" : " invisible pointer-events-none")}
        >
            {message}
            <button
                className="ml-10 underline cursor-pointer"
                onClick={onClose}
            >
                Got it!
            </button>
        </div>
    )
}