"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.Clipboard = void 0;
const bus_1 = require("./bus");
exports.Clipboard = {
    async copy(text, params = {}) {
        await bus_1.bus.request('clipboard-copy', {
            text
        });
    },
    async clear(text) {
        await bus_1.bus.request('clipboard-clear', {
            text
        });
    }
};
