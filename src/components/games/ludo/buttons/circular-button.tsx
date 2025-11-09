export type CircularButtonProps = {
	text: string;
	className: string;
	onClick: () => void;
    isActive: boolean;
};

export default function CircularButton({
	text,
	className,
	isActive,
	onClick,
}: CircularButtonProps) {
	return (
		<button
            className={`rounded-full items-center justify-center size-23 text-4xl ${className} ${isActive ? `${className}-active` : ""}`}
            onClick={onClick}
            type="button"
        >
            {text}
        </button>
	);
}
