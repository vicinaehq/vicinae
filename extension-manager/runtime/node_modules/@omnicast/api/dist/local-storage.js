"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.LocalStorage = void 0;
const bus_1 = require("./bus");
;
class LocalStorage {
    static async getItem(key) {
        const res = await bus_1.bus.request('storage.get', {
            key
        });
        return res.data.value;
    }
    static async setItem(key, value) {
        await bus_1.bus.request('storage.set', { key, value });
    }
    static async removeItem(key) {
        await bus_1.bus.request('storage.remove', { key });
    }
    static async allItems() {
        const res = await bus_1.bus.request('storage.all');
        return res.data.values;
    }
    static async clear() {
        await bus_1.bus.request('storage.clear');
    }
}
exports.LocalStorage = LocalStorage;
