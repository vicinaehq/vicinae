"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.Cache = void 0;
class Cache {
    constructor(options) { }
    /**
     * @returns the full path to the directory where the data is stored on disk.
     */
    get storageDirectory() { return ''; }
    /**
     * @returns the data for the given key. If there is no data for the key, `undefined` is returned.
     * @remarks If you want to just check for the existence of a key, use {@link has}.
     */
    get(key) { return undefined; }
    /**
     * @returns `true` if data for the key exists, `false` otherwise.
     * @remarks You can use this method to check for entries without affecting the LRU access.
     */
    has(key) { return false; }
    /**
     * @returns `true` if the cache is empty, `false` otherwise.
     */
    get isEmpty() { return true; }
    /**
     * Sets the data for the given key.
     * If the data exceeds the configured `capacity`, the least recently used entries are removed.
     * This also notifies registered subscribers (see {@link subscribe}).
     */
    set(key, data) { }
    /**
     * Removes the data for the given key.
     * This also notifies registered subscribers (see {@link subscribe}).
     * @returns `true` if data for the key was removed, `false` otherwise.
     */
    remove(key) { return false; }
    /**
     * Clears all stored data.
     * This also notifies registered subscribers (see {@link subscribe}) unless the  `notifySubscribers` option is set to `false`.
     */
    clear(options) { }
    /**
     * Registers a new subscriber that gets notified when cache data is set or removed.
     * @returns a function that can be called to remove the subscriber.
     */
    subscribe(subscriber) {
        return () => { };
    }
    maintainCapacity;
    notifySubscribers;
}
exports.Cache = Cache;
;
