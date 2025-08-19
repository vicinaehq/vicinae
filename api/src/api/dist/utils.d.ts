/// <reference types="node" />
import { PathLike } from 'fs';
export declare const captureException: (exception: unknown) => void;
export declare const trash: (path: PathLike | PathLike[]) => Promise<void>;
export declare const open: (target: string, app?: Application | string) => void;
export type Application = {
    id: string;
    name: string;
    icon: {
        iconName: string;
    };
};
export declare const getFrontmostApplication: () => Promise<Application>;
export declare const getApplications: (path?: PathLike) => Promise<Application[]>;
export declare const getDefaultApplication: (path: PathLike) => Promise<Application>;
export declare const showInFinder: (path: PathLike) => Promise<void>;
//# sourceMappingURL=utils.d.ts.map