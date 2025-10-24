export type LudoLayoutProps = {
    children: React.ReactNode;
}

export default function LudoLayout({ children }: LudoLayoutProps) {
    return (
        <div>
            {children}
        </div>
    )
}