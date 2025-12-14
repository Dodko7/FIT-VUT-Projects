export type CircularButtonProps = {
	text: string;
	className: string;
	onClick: () => void;
    isActive: boolean;
	size?: number;
};

export default function CircularButton({
	text,
	className,
	isActive,
	onClick,
	size,
}: CircularButtonProps) {
	return (
		<button
            className={`rounded-full items-center justify-center ${size ? `size-${size}` : "size-23"} text-4xl cursor-pointer ${className} ${isActive ? `${className}-active` : ""}`}
            onClick={onClick}
			disabled={isActive}
            type="button"
        >
            {text}
        </button>
	);
}
