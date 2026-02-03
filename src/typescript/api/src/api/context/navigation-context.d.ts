import { ReactNode } from "react";
export type NavigationContextType = {
	push: (node: ReactNode) => void;
	pop: () => void;
};
declare const ctx: import("react").Context<NavigationContextType>;
export default ctx;
//# sourceMappingURL=navigation-context.d.ts.map
