import type React from "react";
import type { Client } from "./proto/api";
import type { Environment } from "./environment";
import type { NavigationContextType } from "./context/navigation-context";

export interface VicinaGlobal {
	client: Client;
	environ: Environment;
	preferences: any;
	navigationContext: React.Context<NavigationContextType>;
}

export const getGlobal = () => (globalThis as any).vicinae as VicinaGlobal;
