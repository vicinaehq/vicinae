import type { Environment } from "@vicinae/api";

export type Global = typeof globalThis & {
	vicinae: {
		environ: Environment;
		preferences: any;
	};
};

(globalThis as Global).vicinae = {
	environ: {} as any,
	preferences: {} as any,
};
