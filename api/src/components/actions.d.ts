import React, { ReactNode } from "react";
export type CopyToClipboardProps = {
    title: string;
    content: string;
};
export type ActionPushProps = {
    target: ReactNode;
};
declare const Action: {
    CopyToClipboard: React.FC<CopyToClipboardProps>;
    Push: React.FC<ActionPushProps>;
};
export default Action;
//# sourceMappingURL=actions.d.ts.map