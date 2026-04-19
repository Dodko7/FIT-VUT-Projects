/** @type {import('prettier').Config & import('prettier-plugin-tailwindcss').PluginOptions} */
export default {
	plugins: ["prettier-plugin-tailwindcss"],
	experimentalTernaries: true,
	experimentalOperatorPosition: "end",
	printWidth: 80,
	tabWidth: 4,
	useTabs: true,
	semi: true,
	singleQuote: false,
	quoteProps: "consistent",
	jsxSingleQuote: false,
	trailingComma: "all",
	bracketSpacing: true,
	objectWrap: "preserve",
	bracketSameLine: false,
	arrowParens: "always",
	singleAttributePerLine: true,
};
