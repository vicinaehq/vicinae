import type { VicinaGlobal } from "@vicinae/api";

type Global = typeof globalThis & {
	vicinae: VicinaGlobal;
};

(globalThis as Global).vicinae = {
	environ: {} as any,
	preferences: {} as any,
	client: {} as any,
	navigationContext: undefined as any,
};

export const globalState = (globalThis as Global).vicinae;
