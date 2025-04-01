import { bus } from './bus';

// Implementation of Raycast's storage API: https://developers.raycast.com/api-reference/storage

export namespace LocalStorage {
	export type Value = string | number | boolean;
	export type Values = { [key: string]: Value };
};

class LocalStorage {
	static async getItem(key: string): Promise<LocalStorage.Value | undefined> {
		const res = await bus.request<{ value: LocalStorage.Value | undefined }>('storage.get', {
			key
		});

		return res.data.value;
	}

	static async setItem(key: string, value: LocalStorage.Value): Promise<void> {
		await bus.request('storage.set', { key, value });
	}

	static async removeItem(key: string): Promise<void> {
		await bus.request('storage.remove', { key });
	}

	static async allItems(): Promise<LocalStorage.Values> {
		const res = await bus.request<{ values: LocalStorage.Values }>('storage.all');

		return res.data.values;
	}

	static async clear(): Promise<void> {
		await bus.request('storage.clear');
	}
}
