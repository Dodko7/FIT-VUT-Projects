"use client";

import StatusIndicatorState from "~/lib/ludo/enum/status-indicator-state";
import type { StatusIndicatorProps } from "~/lib/ludo/types";

export function StatusIndicator({
	type,
	message,
	isVisible,
}: StatusIndicatorProps) {
	const baseClasses = `fixed top-4 left-1/2 transform -translate-x-1/2 p-3 px-6 rounded-lg shadow-2xl transition-opacity duration-300 z-50 
                         ${isVisible ? "opacity-100" : "opacity-0 pointer-events-none"}`;
	let colorClasses = "";

	// Gray-ish/transparent to not be intrusive
	switch (type) {
		case StatusIndicatorState.SUCCESS:
			colorClasses = "bg-green-500/80 text-white";
			break;
		case StatusIndicatorState.ERROR:
			colorClasses = "bg-red-600/80 text-white";
			break;
		case StatusIndicatorState.LOADING:
			colorClasses = "bg-gray-700/70 text-gray-100";
			break;
		// Will never happen btw
		default:
			return null;
	}

	// Loading Spinner
	const Spinner = () => (
		<svg
			className="mr-3 -ml-1 h-5 w-5 animate-spin text-white"
			xmlns="http://www.w3.org/2000/svg"
			fill="none"
			viewBox="0 0 24 24"
		>
			<circle
				className="opacity-25"
				cx="12"
				cy="12"
				r="10"
				stroke="currentColor"
				strokeWidth="4"
			></circle>
			<path
				className="opacity-75"
				fill="currentColor"
				d="M4 12a8 8 0 018-8V0C5.373 0 0 5.373 0 12h4zm2 5.291A7.962 7.962 0 014 12H0c0 3.042 1.135 5.824 3 7.938l3-2.647z"
			></path>
		</svg>
	);

	return (
		<div className={`${baseClasses} ${colorClasses} flex items-center`}>
			{type === StatusIndicatorState.LOADING && <Spinner />}
			<span className="text-sm font-medium whitespace-nowrap">
				{message}
			</span>
		</div>
	);
}
