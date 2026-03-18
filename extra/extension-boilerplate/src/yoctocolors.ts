// Minimal yoctocolors — always emit ANSI codes (no TTY check needed for Vicinae rendering).
const format = (open: number, close: number) => {
	const openCode = `\u001B[${open}m`;
	const closeCode = `\u001B[${close}m`;
	return (input: string = "") => openCode + input + closeCode;
};

export const reset = format(0, 0);
export const bold = format(1, 22);
export const dim = format(2, 22);
export const italic = format(3, 23);
export const underline = format(4, 24);
export const strikethrough = format(9, 29);

export const red = format(31, 39);
export const green = format(32, 39);
export const yellow = format(33, 39);
export const blue = format(34, 39);
export const magenta = format(35, 39);
export const cyan = format(36, 39);

export const redBright = format(91, 39);
export const greenBright = format(92, 39);
export const yellowBright = format(93, 39);
export const blueBright = format(94, 39);
export const magentaBright = format(95, 39);
export const cyanBright = format(96, 39);

export const bgRed = format(41, 49);
export const bgGreen = format(42, 49);
export const bgBlue = format(44, 49);
export const bgYellow = format(43, 49);
export const bgMagenta = format(45, 49);
export const bgCyan = format(46, 49);
