export * from './module/core';
export * from './module/duplex';
export {
    PatchError as JsonPatchError,
    _deepClone as deepClone,
    escapePathComponent,
    unescapePathComponent
} from './module/helpers';


/**
 * Default export for backwards compat
 */
import * as core from './module/core';
import * as duplex from './module/duplex';
import { PatchError as JsonPatchError, _deepClone as deepClone, escapePathComponent, unescapePathComponent } from './module/helpers';
declare const _default: {
    JsonPatchError: typeof JsonPatchError;
    deepClone: typeof deepClone;
    escapePathComponent: typeof escapePathComponent;
    unescapePathComponent: typeof unescapePathComponent;
    unobserve<T>(root: T, observer: duplex.Observer<T>): void;
    observe<T>(obj: Object | T[], callback?: (patches: core.Operation[]) => void): duplex.Observer<T>;
    generate<T>(observer: duplex.Observer<Object>, invertible?: boolean): core.Operation[];
    compare(tree1: Object | any[], tree2: Object | any[], invertible?: boolean): core.Operation[];
    getValueByPointer(document: any, pointer: string): any;
    applyOperation<T>(document: T, operation: core.Operation, validateOperation?: boolean | core.Validator<T>, mutateDocument?: boolean, banPrototypeModifications?: boolean, index?: number): core.OperationResult<T>;
    applyPatch<T>(document: T, patch: core.Operation[], validateOperation?: boolean | core.Validator<T>, mutateDocument?: boolean, banPrototypeModifications?: boolean): core.PatchResult<T>;
    applyReducer<T>(document: T, operation: core.Operation, index: number): T;
    validator(operation: core.Operation, index: number, document?: any, existingPathFragment?: string): void;
    validate<T>(sequence: core.Operation[], document?: T, externalValidator?: core.Validator<T>): JsonPatchError;
    _areEquals(a: any, b: any): boolean;
};
export default _default;