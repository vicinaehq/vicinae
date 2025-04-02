import React, { ReactNode } from "react";
import { ImageLike } from "../image";
import { Keyboard } from "../keyboard";
import { Application } from "../utils";
export type BaseActionProps = {
    title: string;
    icon?: ImageLike;
    shortcut?: Keyboard.Shortcut;
};
export type ActionProps = BaseActionProps & {
    onAction: () => void;
};
export type CopyToClipboardProps = BaseActionProps & {
    content: string;
};
export type ActionPushProps = BaseActionProps & {
    target: ReactNode;
};
export type ActionOpenProps = BaseActionProps & {
    target: string;
    app?: Application;
};
export declare const Action: React.FC<ActionProps> & {
    CopyToClipboard: React.FC<CopyToClipboardProps>;
    Push: React.FC<ActionPushProps>;
    Open: React.FC<ActionOpenProps>;
};
//# sourceMappingURL=actions.d.ts.map