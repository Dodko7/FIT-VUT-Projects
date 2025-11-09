export default function LudoMenuLayout({
	children,
}: {
	children: React.ReactNode;
}) {
	return <div
        className="flex flex-col w-screen h-screen ludo-bg"
    >{children}</div>;
}
