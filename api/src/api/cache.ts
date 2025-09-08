import { join } from "node:path";
import { existsSync, mkdirSync, readFileSync, rmSync, writeFileSync } from "node:fs";
import { hash } from "node:crypto";
import { environment } from "./environment";

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
  export type Subscriber = (
    key: string | undefined,
    data: string | undefined,
  ) => void;
  export type Subscription = () => void;
}

type CacheKeyInfo = {
	id: string;
	size: number;
};

type CacheIndex = {
	revision: string;
	size: number;
	keys: Record<string, CacheKeyInfo>;
	lru: Array<{ key: string, lastUsedAt: number }>;
};

export class Cache {
  constructor(options?: Cache.Options) {
	  const cacheDir = join(environment.supportPath, '.cache');

	  if (options?.namespace) {
		  this.storageDir = join(cacheDir, options.namespace);
	  } else {
		  this.storageDir = cacheDir;
	  }

	  if (options?.capacity) {
		  this.capacity = options.capacity;
	  }

	  const idx = this.loadIndex();

	  if (!idx) { 
	  	  this.index = this.initIndex();
		  return ;
	  }

	  if (idx.revision !== this.revision) {
		this.clear();
		this.index = this.initIndex();
		return ;
	  }
	
	  this.index = idx;
  }

  /**
   * @returns the full path to the directory where the data is stored on disk.
   */
  get storageDirectory(): string {
    return this.storageDir;
  }

  /**
   * @returns the data for the given key. If there is no data for the key, `undefined` is returned.
   * @remarks If you want to just check for the existence of a key, use {@link has}.
   */
  get(key: string): string | undefined {
	const info = this.index.keys[key];

	if (!info) return undefined;

	this.updateLRU(key);
	this.syncIndex();

	return readFileSync(this.dataPath(info.id)).toString();
  }

  /**
   * @returns `true` if data for the key exists, `false` otherwise.
   * @remarks You can use this method to check for entries without affecting the LRU access.
   */
  has(key: string): boolean {
    return typeof this.index.keys[key] !== 'undefined';
  }

  /**
   * @returns whether the cache is empty.
   */
  get isEmpty(): boolean {
    return this.index.size === 0;
  }

  /**
   * Sets the data for the given key.
   * If the data exceeds the configured `capacity`, the least recently used entries are removed.
   * This also notifies registered subscribers (see {@link subscribe}).
   */
  set(key: string, data: string): void {
	if (data.length > this.capacity) {
		throw new Error(`A single key cannot be bigger than the total capacity of the cache. The data for key ${key} is ${data.length} bytes long while the capacity is set to ${this.capacity}.`);
	}

	const info = this.index.keys[key];
	let newTotalSize = this.index.size + data.length - (info?.size ?? 0);

	if (newTotalSize > this.capacity) {
		this.popLRU();
		return this.set(key, data); // FIXME: get rid of recursion 
	}

	this.index.size = newTotalSize;
	this.index.keys[key] = { id: '', size: data.length };
	this.updateLRU(key);

	for (const sub of this.subscribers) {
		sub(key, data);
	}

	this.writeKeyData(key, data);
	this.syncIndex();
  }

  /**
   * Removes the data for the given key.
   * This also notifies registered subscribers (see {@link subscribe}).
   * @returns `true` if data for the key was removed, `false` otherwise.
   */
  remove(key: string): boolean {
	const removed = this.removeImpl(key);
	this.syncIndex()

	return removed;
  }

  /**
   * Clears all stored data.
   * This also notifies registered subscribers (see {@link subscribe}) unless the  `notifySubscribers` option is set to `false`.
   */
  clear(options?: { notifySubscribers: boolean }): void {
	  // TODO: notify subscribers
	  rmSync(this.dataDir, { recursive: true, force: true });
	  this.initIndex();
  }

  /**
   * Registers a new subscriber that gets notified when cache data is set or removed.
   * @returns a function that can be called to remove the subscriber.
   */
  subscribe(subscriber: Cache.Subscriber): Cache.Subscription {
	this.subscribers.push(subscriber);

    return () => {
		this.subscribers.splice(this.subscribers.indexOf(subscriber), 1);
	};
  }

  private keyDataPath(key: string) {
	  return this.dataPath(this.keyHash(key));
  }

  private readKeyData(key: string): string {
	  return readFileSync(this.keyDataPath(key), 'utf8');
  }

  private writeKeyData(key: string, data: string): void {
	  writeFileSync(this.keyDataPath(key), data);
  }

  private keyHash(key: string) {
	  return hash('md5', key);
  }
  
  private get dataDir() {
	  return join(this.storageDir, 'data');
  }

  private dataPath(id: string) {
	  return join(this.dataDir, id);
  }

  private get indexPath() {
	  return join(this.storageDir, 'index.json');
  }

  private updateLRU(key: string) {
	const idx = this.index.lru.findIndex((entry) => key == entry.key);

	if (idx !== -1) this.index.lru.splice(idx, 1);

	this.index.lru.unshift({ key, lastUsedAt: Date.now() });
  }

  private popLRU() {
	  if (this.index.lru.length === 0) return ;

	  const key = this.index.lru.pop()![0];
	  this.removeImpl(key);
  }

  private removeImpl(key: string) {
	const info = this.index.keys[key];

	if (!info) return false;

	this.index.size -= info.size;
	delete this.index.keys[key];

	for (const sub of this.subscribers) {
		sub(key, undefined);
	}

    return true;
  }

  private initIndex() {
	  const index = { revision: this.revision, keys: {}, size: 0, lru: [] } as CacheIndex;

	  mkdirSync(this.dataDir , { recursive: true });
	  const indexPath = join(this.storageDir, "index.json");
	  writeFileSync(indexPath, JSON.stringify(index, null, 2));
	  return index;
  }

  private loadIndex(): CacheIndex | null {
	  const indexPath = join(this.storageDir, "index.json");

	  if (!existsSync(indexPath)) return null;

	  return JSON.parse(readFileSync(indexPath, 'utf8'));
  }

  private syncIndex() {
	  writeFileSync(this.indexPath, JSON.stringify(this.index, null, 2));
  }

  /**
   * We store this inside the cache index file in order to know
   * when a breaking change occurs.
   * If we want to change the way the data is stored we just change this,
   * which will force a full cache clear and use the new format.
   */
  private revision = '1';

  private capacity = 1e7;
  private subscribers: Cache.Subscriber[] = [];
  private storageDir: string;
  private index: CacheIndex;
}


