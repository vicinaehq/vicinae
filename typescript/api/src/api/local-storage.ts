import { bus } from "./bus";

// Implementation of Raycast's storage API: https://developers.raycast.com/api-reference/storage

/**
 * @category Local Storage
 */
export namespace LocalStorage {
	export type Value = string | number | boolean;
	export type Values = { [key: string]: Value };

	export async function getItem<T extends LocalStorage.Value>(
		key: string,
	): Promise<T | undefined> {
		const res = await bus.request("storage.get", { key });

		if (!res.ok || res.value.value === null) {
			return undefined;
		}

		return res.value.value;
	}

	export async function setItem(
		key: string,
		value: LocalStorage.Value,
	): Promise<void> {
		await bus.request("storage.set", { key, value });
	}

	export async function removeItem(key: string): Promise<void> {
		await bus.request("storage.remove", { key });
	}

	export async function allItems(): Promise<LocalStorage.Values> {
		const res = await bus.request("storage.list", {});

		if (!res.ok) return {};

		return res.value.values;
	}

	export async function clear(): Promise<void> {
		await bus.request("storage.clear", {});
	}
}
