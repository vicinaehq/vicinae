import type React from "react";
import type { Environment } from "./environment";
import type { NavigationContextType } from "./context/navigation-context";

export interface VicinaGlobal {
	client: unknown;
	environ: Environment;
	preferences: any;
	navigationContext: React.Context<NavigationContextType>;
}

export const getGlobal = () => (globalThis as any).vicinae as VicinaGlobal;
