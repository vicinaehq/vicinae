import type { Environment } from "@vicinae/api";
import type { Client } from "./proto/api";
import type { createRenderer } from "./reconciler";

type VicinaGlobal = {
	client: Client;
	environ: Environment;
	preferences: any;
	navigationContext: any;
	renderer?: ReturnType<typeof createRenderer>;
};

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
