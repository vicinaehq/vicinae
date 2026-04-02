import { createContext, type ReactNode } from "react";
import { getGlobal } from "../globals";

export type NavigationContextType = {
	push: (node: ReactNode) => void;
	pop: () => void;
};

export function getNavigationContext() {
	const g = getGlobal();
	if (!g.navigationContext) {
		g.navigationContext = createContext<NavigationContextType>({
			pop: () => {
				throw new Error("not implemented");
			},
			push: () => {
				throw new Error("not implemented");
			},
		});
	}
	return g.navigationContext;
}
