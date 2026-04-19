export type MenuHeaderProps = {
	title: string;
	component?: React.ReactNode;
	className?: string;
};

export default function LudoMenuHeader({
	title,
	component,
	className,
}: MenuHeaderProps) {
	return (
		<div
			className={`ludo-text-primary ludo-header flex w-full items-center justify-start gap-4 px-5 text-5xl ${className ?? ""}`}
		>
			{component}
			{title}
		</div>
	);
}
