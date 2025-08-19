export declare namespace LocalStorage {
    type Value = string | number | boolean;
    type Values = {
        [key: string]: Value;
    };
}
export declare class LocalStorage {
    static getItem<T extends LocalStorage.Value>(key: string): Promise<T | undefined>;
    static setItem(key: string, value: LocalStorage.Value): Promise<void>;
    static removeItem(key: string): Promise<void>;
    static allItems(): Promise<LocalStorage.Values>;
    static clear(): Promise<void>;
}
//# sourceMappingURL=local-storage.d.ts.map