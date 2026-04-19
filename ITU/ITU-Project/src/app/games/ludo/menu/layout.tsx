export default function LudoMenuLayout({
	children,
}: {
	children: React.ReactNode;
}) {
	return <div
        className="flex flex-col w-screen ludo-bg h-screen"
    >{children}</div>;
}
