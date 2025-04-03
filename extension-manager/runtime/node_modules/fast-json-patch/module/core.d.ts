import { PatchError, _deepClone } from './helpers.js';
export declare const JsonPatchError: typeof PatchError;
export declare const deepClone: typeof _deepClone;
export declare type Operation = AddOperation<any> | RemoveOperation | ReplaceOperation<any> | MoveOperation | CopyOperation | TestOperation<any> | GetOperation<any>;
export interface Validator<T> {
    (operation: Operation, index: number, document: T, existingPathFragment: string): void;
}
export interface OperationResult<T> {
    removed?: any;
    test?: boolean;
    newDocument: T;
}
export interface BaseOperation {
    path: string;
}
export interface AddOperation<T> extends BaseOperation {
    op: 'add';
    value: T;
}
export interface RemoveOperation extends BaseOperation {
    op: 'remove';
}
export interface ReplaceOperation<T> extends BaseOperation {
    op: 'replace';
    value: T;
}
export interface MoveOperation extends BaseOperation {
    op: 'move';
    from: string;
}
export interface CopyOperation extends BaseOperation {
    op: 'copy';
    from: string;
}
export interface TestOperation<T> extends BaseOperation {
    op: 'test';
    value: T;
}
export interface GetOperation<T> extends BaseOperation {
    op: '_get';
    value: T;
}
export interface PatchResult<T> extends Array<OperationResult<T>> {
    newDocument: T;
}
/**
 * Retrieves a value from a JSON document by a JSON pointer.
 * Returns the value.
 *
 * @param document The document to get the value from
 * @param pointer an escaped JSON pointer
 * @return The retrieved value
 */
export declare function getValueByPointer(document: any, pointer: string): any;
/**
 * Apply a single JSON Patch Operation on a JSON document.
 * Returns the {newDocument, result} of the operation.
 * It modifies the `document` and `operation` objects - it gets the values by reference.
 * If you would like to avoid touching your values, clone them:
 * `jsonpatch.applyOperation(document, jsonpatch._deepClone(operation))`.
 *
 * @param document The document to patch
 * @param operation The operation to apply
 * @param validateOperation `false` is without validation, `true` to use default jsonpatch's validation, or you can pass a `validateOperation` callback to be used for validation.
 * @param mutateDocument Whether to mutate the original document or clone it before applying
 * @param banPrototypeModifications Whether to ban modifications to `__proto__`, defaults to `true`.
 * @return `{newDocument, result}` after the operation
 */
export declare function applyOperation<T>(document: T, operation: Operation, validateOperation?: boolean | Validator<T>, mutateDocument?: boolean, banPrototypeModifications?: boolean, index?: number): OperationResult<T>;
/**
 * Apply a full JSON Patch array on a JSON document.
 * Returns the {newDocument, result} of the patch.
 * It modifies the `document` object and `patch` - it gets the values by reference.
 * If you would like to avoid touching your values, clone them:
 * `jsonpatch.applyPatch(document, jsonpatch._deepClone(patch))`.
 *
 * @param document The document to patch
 * @param patch The patch to apply
 * @param validateOperation `false` is without validation, `true` to use default jsonpatch's validation, or you can pass a `validateOperation` callback to be used for validation.
 * @param mutateDocument Whether to mutate the original document or clone it before applying
 * @param banPrototypeModifications Whether to ban modifications to `__proto__`, defaults to `true`.
 * @return An array of `{newDocument, result}` after the patch
 */
export declare function applyPatch<T>(document: T, patch: ReadonlyArray<Operation>, validateOperation?: boolean | Validator<T>, mutateDocument?: boolean, banPrototypeModifications?: boolean): PatchResult<T>;
/**
 * Apply a single JSON Patch Operation on a JSON document.
 * Returns the updated document.
 * Suitable as a reducer.
 *
 * @param document The document to patch
 * @param operation The operation to apply
 * @return The updated document
 */
export declare function applyReducer<T>(document: T, operation: Operation, index: number): T;
/**
 * Validates a single operation. Called from `jsonpatch.validate`. Throws `JsonPatchError` in case of an error.
 * @param {object} operation - operation object (patch)
 * @param {number} index - index of operation in the sequence
 * @param {object} [document] - object where the operation is supposed to be applied
 * @param {string} [existingPathFragment] - comes along with `document`
 */
export declare function validator(operation: Operation, index: number, document?: any, existingPathFragment?: string): void;
/**
 * Validates a sequence of operations. If `document` parameter is provided, the sequence is additionally validated against the object document.
 * If error is encountered, returns a JsonPatchError object
 * @param sequence
 * @param document
 * @returns {JsonPatchError|undefined}
 */
export declare function validate<T>(sequence: ReadonlyArray<Operation>, document?: T, externalValidator?: Validator<T>): PatchError;
export declare function _areEquals(a: any, b: any): boolean;
