import type { Environment } from "@vicinae/api";
import type { Client } from "./proto/api";

export type Global = typeof globalThis & {
	vicinae: {
		environ: Environment;
		preferences: any;
		client: Client;
	};
};

(globalThis as Global).vicinae = {
	environ: {} as any,
	preferences: {} as any,
	client: {} as any,
};

export const globalState = (globalThis as Global).vicinae;
