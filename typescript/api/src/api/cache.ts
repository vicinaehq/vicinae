import path from "node:path";
import {
	existsSync,
	mkdirSync,
	readFileSync,
	rmSync,
	writeFileSync,
} from "node:fs";
import { hash } from "node:crypto";
import { environment } from "./environment";

/**
 * @category Cache
 */
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
		 * The capacity in bytes. If the stored data exceeds the capacity, the least recently used data is removed.
		 * The default capacity is 10 MB.
		 */
		capacity?: number;
	}
	export type Subscriber = (
		key: string | undefined,
		data: string | undefined,
	) => void;
	export type Subscription = () => void;
}

type CacheKeyInfo = {
	size: number;
};

type CacheIndex = {
	revision: string;
	size: number;
	keys: Record<string, CacheKeyInfo>;
	lru: Array<{ key: string; lastUsedAt: number }>;
};

/**
 * Caching abstraction that stores data on disk and supports LRU (least recently used) access.
 * Values can only be stored as plain text strings, so it is up to you to serialize your data in an appropriate way.
 * For instance, you could store json using `JSON.stringify` and `JSON.parse` it back.
 * If you need to store binary data, you could encode it in base64.
 *
 * Unlike the local storage API, this API exclusively uses the extension's support directory to store its data.
 * No calls to internal Vicinae APIs are required, hence why all methods in this class are synchronous.
 * Another major difference is that cache data is not encrypted, so it's not suitable to store secrets.
 * (local storage currently isn't either, but that will change in the future)
 *
 * @category Cache
 */
export class Cache {
	constructor(options?: Cache.Options) {
		this.storageDir = path.join(environment.supportPath, Cache.CACHE_DIR_NAME);
		this.capacity = options?.capacity ?? Cache.DEFAULT_CACHE_SIZE;

		if (options?.namespace) {
			this.storageDir = path.join(this.storageDir, options.namespace);
		}

		mkdirSync(this.dataDir, { recursive: true });
		this.index = this.loadIndex() ?? this.emptyIndex();

		if (this.index.revision !== this.revision) {
			this.clear();
		}
	}

	/**
	 * @returns the full path to the directory where the data is stored on disk.
	 * @remarks This is provided for informative purpose only. You should not attempt to mutate anything
	 * in this directory by yourself. Also note that the way cache data is serialized on disk can (will) change
	 * across versions.
	 */
	get storageDirectory(): string {
		return this.storageDir;
	}

	/**
	 * @returns the data for the given key, or `undefined` if there is no data.
	 * @remarks To solely check for existence of a key, use {@link has}.
	 */
	get = (key: string): string | undefined => {
		const info = this.index.keys[key];

		if (!info) return undefined;

		this.updateLRU(key);
		this.syncIndex();

		return this.readKeyData(key);
	};

	/**
	 * @returns `true` if data for the key exists, `false` otherwise.
	 * @remarks You can use this method to check for entries without affecting the LRU access.
	 */
	has = (key: string): boolean => {
		return typeof this.index.keys[key] !== "undefined";
	};

	/**
	 * @returns whether the cache is empty.
	 */
	get isEmpty(): boolean {
		return Object.keys(this.index.keys).length === 0;
	}

	/**
	 * Sets the data for the given key.
	 * If the data exceeds the configured `capacity`, the least recently used entries are removed.
	 * This also notifies registered subscribers (see {@link subscribe}).
	 * @remarks An individual cache entry cannot be bigger than the configured capacity. If this happens, an error will be thrown.
	 */
	set = (key: string, data: string): void => {
		if (data.length > this.capacity) {
			throw new Error(
				`A single cache entry cannot be bigger than the total capacity of the cache. The data for key ${key} is ${data.length} bytes long while the capacity is set to ${this.capacity}. You should either reduce the amount of data stored or increase the cache's capacity.`,
			);
		}

		const info = this.index.keys[key];
		const newTotalSize = this.index.size + data.length - (info?.size ?? 0);

		if (newTotalSize > this.capacity) {
			this.popLRU();
			this.set(key, data); // FIXME: get rid of recursion
			return;
		}

		this.index.size = newTotalSize;
		this.index.keys[key] = { size: data.length };
		this.updateLRU(key);
		this.writeKeyData(key, data);
		this.syncIndex();

		for (const sub of this.subscribers) {
			sub(key, data);
		}
	};

	/**
	 * Removes the data for the given key.
	 * This also notifies registered subscribers (see {@link subscribe}).
	 * @returns `true` if data for the key was removed, `false` otherwise.
	 */
	remove = (key: string): boolean => {
		const removed = this.removeImpl(key);
		this.syncIndex();

		return removed;
	};

	/**
	 * Clears all stored data.
	 * This also notifies registered subscribers (see {@link subscribe}) unless the  `notifySubscribers` option is set to `false`.
	 */
	clear = (options?: { notifySubscribers: boolean }): void => {
		const notify = options?.notifySubscribers ?? true;

		this.removeCacheDirectory();
		this.initIndex();

		if (!notify) return;

		for (const key of Object.keys(this.index.keys)) {
			for (const subscriber of this.subscribers) {
				subscriber(key, undefined);
			}
		}
	};

	/**
	 * Registers a new subscriber that gets notified when cache data is set or removed.
	 * @returns a function that can be called to remove the subscriber.
	 */
	subscribe = (subscriber: Cache.Subscriber): Cache.Subscription => {
		this.subscribers.push(subscriber);

		return () => {
			this.subscribers.splice(this.subscribers.indexOf(subscriber), 1);
		};
	};

	private keyDataPath(key: string) {
		return this.dataPath(this.keyHash(key));
	}

	private readKeyData(key: string): string {
		return readFileSync(this.keyDataPath(key), "utf8");
	}

	private writeKeyData(key: string, data: string): void {
		writeFileSync(this.keyDataPath(key), data);
	}

	private keyHash(key: string) {
		return hash("md5", key);
	}

	private get dataDir() {
		return path.join(this.storageDir, "data");
	}

	private dataPath(id: string) {
		return path.join(this.dataDir, id);
	}

	private get indexPath() {
		return path.join(this.storageDir, "index.json");
	}

	private updateLRU(key: string) {
		const idx = this.index.lru.findIndex((entry) => key == entry.key);

		if (idx !== -1) this.index.lru.splice(idx, 1);

		this.index.lru.unshift({ key, lastUsedAt: Date.now() });
	}

	private popLRU() {
		if (this.index.lru.length === 0) return;
		this.removeImpl(this.index.lru.pop()!.key);
	}

	private removeImpl(key: string) {
		const info = this.index.keys[key];

		if (!info) return false;

		rmSync(this.keyDataPath(key), { force: true });
		this.index.size -= info.size;
		delete this.index.keys[key];

		for (const sub of this.subscribers) {
			sub(key, undefined);
		}

		return true;
	}

	private initIndex() {
		const index = {
			revision: this.revision,
			keys: {},
			size: 0,
			lru: [],
		} as CacheIndex;
		mkdirSync(this.dataDir, { recursive: true });
		const indexPath = path.join(this.storageDir, "index.json");
		writeFileSync(indexPath, JSON.stringify(index, null, 2));
		return index;
	}

	private loadIndex(): CacheIndex | null {
		const indexPath = path.join(this.storageDir, "index.json");

		if (!existsSync(indexPath)) return null;

		return JSON.parse(readFileSync(indexPath, "utf8"));
	}

	private removeCacheDirectory() {
		rmSync(this.dataDir, { recursive: true, force: true });
	}

	private syncIndex() {
		writeFileSync(this.indexPath, JSON.stringify(this.index, null, 2));
	}

	private emptyIndex() {
		return { revision: this.revision, keys: {}, lru: [], size: 0 };
	}

	/**
	 * We store this inside the cache index file in order to know
	 * when a breaking change occurs.
	 * If we want to change the way the data is stored we just change this,
	 * which will force a full cache clear and use the new format.
	 */
	private revision = "1";
	private capacity: number;
	private subscribers: Cache.Subscriber[] = [];
	private storageDir: string;
	private index: CacheIndex;
	private static DEFAULT_CACHE_SIZE = 1e7;
	private static CACHE_DIR_NAME = ".cache";
}
