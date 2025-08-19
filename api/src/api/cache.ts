interface AbstractCache {
    /**
     * @returns the full path to the directory where the data is stored on disk.
     */
    get storageDirectory(): string;
    /**
     * @returns the data for the given key. If there is no data for the key, `undefined` is returned.
     * @remarks If you want to just check for the existence of a key, use {@link has}.
     */
    get(key: string): string | undefined;
    /**
     * @returns `true` if data for the key exists, `false` otherwise.
     * @remarks You can use this method to check for entries without affecting the LRU access.
     */
    has(key: string): boolean;
    /**
     * @returns `true` if the cache is empty, `false` otherwise.
     */
    get isEmpty(): boolean;
    /**
     * Sets the data for the given key.
     * If the data exceeds the configured `capacity`, the least recently used entries are removed.
     * This also notifies registered subscribers (see {@link subscribe}).
     */
    set(key: string, data: string): void;
    /**
     * Removes the data for the given key.
     * This also notifies registered subscribers (see {@link subscribe}).
     * @returns `true` if data for the key was removed, `false` otherwise.
     */
    remove(key: string): boolean;
    /**
     * Clears all stored data.
     * This also notifies registered subscribers (see {@link subscribe}) unless the  `notifySubscribers` option is set to `false`.
     */
    clear(options?: {
        notifySubscribers: boolean;
    }): void;
    /**
     * Registers a new subscriber that gets notified when cache data is set or removed.
     * @returns a function that can be called to remove the subscriber.
     */
    subscribe(subscriber: Cache.Subscriber): Cache.Subscription;
}

export class Cache implements AbstractCache {
	constructor(options?: Cache.Options) {}

    /**
     * @returns the full path to the directory where the data is stored on disk.
     */
    get storageDirectory(): string { return ''; }
    /**
     * @returns the data for the given key. If there is no data for the key, `undefined` is returned.
     * @remarks If you want to just check for the existence of a key, use {@link has}.
     */
    get(key: string): string | undefined { return undefined; }
    /**
     * @returns `true` if data for the key exists, `false` otherwise.
     * @remarks You can use this method to check for entries without affecting the LRU access.
     */
    has(key: string): boolean { return false; }
    /**
     * @returns `true` if the cache is empty, `false` otherwise.
     */
    get isEmpty(): boolean { return true; }
    /**
     * Sets the data for the given key.
     * If the data exceeds the configured `capacity`, the least recently used entries are removed.
     * This also notifies registered subscribers (see {@link subscribe}).
     */
    set(key: string, data: string): void {}
    /**
     * Removes the data for the given key.
     * This also notifies registered subscribers (see {@link subscribe}).
     * @returns `true` if data for the key was removed, `false` otherwise.
     */
    remove(key: string): boolean { return false; }
    /**
     * Clears all stored data.
     * This also notifies registered subscribers (see {@link subscribe}) unless the  `notifySubscribers` option is set to `false`.
     */
    clear(options?: {
        notifySubscribers: boolean;
    }): void {}
    /**
     * Registers a new subscriber that gets notified when cache data is set or removed.
     * @returns a function that can be called to remove the subscriber.
     */
    subscribe(subscriber: Cache.Subscriber): Cache.Subscription {
		return () => {};
	}

    private maintainCapacity;
    private notifySubscribers;
};

export declare namespace Cache {
    /**
     * The options for creating a new {@link Cache}.
     */
    export interface Options {
        /**
         * If set, the Cache will be namespaced via a subdirectory.
         * This can be useful to separate the caches for individual commands of an extension.
         * By default, the cache is shared between the commands of an extension.
         */
        namespace?: string;
        /**
         * The parent directory for the cache data.
         * @deprecated this parameter will be removed in the future – use the default directory.
         */
        directory?: string;
        /**
         * The capacity in bytes. If the stored data exceeds the capacity, the least recently used data is removed.
         * The default capacity is 10 MB.
         */
        capacity?: number;
    }
    export type Subscriber = (key: string | undefined, data: string | undefined) => void;
    export type Subscription = () => void;
}


