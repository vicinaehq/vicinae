import { ReactNode } from "react";
import { ImageLike } from "../image";
export type ActionPanelProps = {
    title?: string;
    children?: ReactNode;
};
export type ActionPanelSectionProps = {
    title?: string;
    children?: ReactNode;
};
export type ActionPanelSubmenuProps = {
    title: string;
    icon?: ImageLike;
    onOpen?: () => void;
    onSearchTextChange: (text: string) => void;
};
export declare const ActionPanel: import("react").FC<ActionPanelProps> & {
    Section: import("react").FC<ActionPanelSectionProps>;
    Submenu: import("react").FC<ActionPanelSubmenuProps>;
};
//# sourceMappingURL=action-pannel.d.ts.map