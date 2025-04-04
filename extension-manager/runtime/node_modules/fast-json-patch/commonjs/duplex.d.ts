import { Operation } from './core.js';
export interface Observer<T> {
    object: T;
    patches: Operation[];
    unobserve: () => void;
    callback: (patches: Operation[]) => void;
}
/**
 * Detach an observer from an object
 */
export declare function unobserve<T>(root: T, observer: Observer<T>): void;
/**
 * Observes changes made to an object, which can then be retrieved using generate
 */
export declare function observe<T>(obj: Object | Array<T>, callback?: (patches: Operation[]) => void): Observer<T>;
/**
 * Generate an array of patches from an observer
 */
export declare function generate<T>(observer: Observer<Object>, invertible?: boolean): Operation[];
/**
 * Create an array of patches from the differences in two objects
 */
export declare function compare(tree1: Object | Array<any>, tree2: Object | Array<any>, invertible?: boolean): Operation[];
