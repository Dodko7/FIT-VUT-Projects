"use client";

export type ControlButtonProps = {
	isUp: boolean;
	onClick: () => void;
};

export default function ControlButton({ isUp, onClick }: ControlButtonProps) {
    return <button
		className={"triangle-button size-10 " + (isUp ? "triangle-button-up" : "triangle-button-down")}
		onClick={onClick}
	/>;
}