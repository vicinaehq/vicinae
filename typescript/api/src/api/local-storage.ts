import { bus } from "./bus";

// Implementation of Raycast's storage API: https://developers.raycast.com/api-reference/storage

export namespace LocalStorage {
	export type Value = string | number | boolean;
	export type Values = { [key: string]: Value };
}

export class LocalStorage {
	static async getItem<T extends LocalStorage.Value>(
		key: string,
	): Promise<T | undefined> {
		const res = await bus.turboRequest("storage.get", { key });

		if (!res.ok || res.value.value === null) {
			return undefined;
		}

		return res.value.value;
	}

	static async setItem(key: string, value: LocalStorage.Value): Promise<void> {
		await bus.turboRequest("storage.set", { key, value });
	}

	static async removeItem(key: string): Promise<void> {
		await bus.turboRequest("storage.remove", { key });
	}

	static async allItems(): Promise<LocalStorage.Values> {
		const res = await bus.turboRequest("storage.list", {});

		if (!res.ok) return {};

		return res.value.values;
	}

	static async clear(): Promise<void> {
		await bus.turboRequest("storage.clear", {});
	}
}
