// Implementation of Raycast's storage API: https://developers.raycast.com/api-reference/storage

import { getClient } from "./client";

/**
 * @category Local Storage
 */
export namespace LocalStorage {
	export type Value = string | number | boolean;
	export type Values = { [key: string]: Value };

	export async function getItem<T extends LocalStorage.Value>(
		key: string,
	): Promise<T | undefined> {
		const res = await getClient().Storage.get(key);

		if (!res.ok || res.value.value === null) {
			return undefined;
		}

		return res.value.value;
	}

	export async function setItem(
		key: string,
		value: LocalStorage.Value,
	): Promise<void> {
		getClient().Storage.set(key, value);
	}

	export async function removeItem(key: string): Promise<void> {
		getClient().Storage.remove(key);
	}

	export async function allItems(): Promise<LocalStorage.Values> {
		return getClient().Storage.list();
	}

	export async function clear(): Promise<void> {
		getClient().Storage.clear();
	}
}
