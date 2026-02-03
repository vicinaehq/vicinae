import { createContext, ReactNode } from "react";

export type NavigationContextType = {
	push: (node: ReactNode) => void;
	pop: () => void;
};

const ctx = createContext<NavigationContextType>({
	pop: () => {
		throw new Error("not implemented");
	},
	push: () => {
		throw new Error("not implemented");
	},
});

export default ctx;
