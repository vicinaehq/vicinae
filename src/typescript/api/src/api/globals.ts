import type React from "react";
import type { Environment } from "./environment";

export interface VicinaGlobal {
	client: any;
	environ: Environment;
	preferences: any;
	navigationContext: React.Context<any>;
}

export const getGlobal = () => (globalThis as any).vicinae as VicinaGlobal;
