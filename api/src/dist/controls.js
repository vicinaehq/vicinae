"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.closeMainWindow = void 0;
const bus_1 = require("./bus");
const closeMainWindow = () => {
    bus_1.bus.request('close-main-window', {});
};
exports.closeMainWindow = closeMainWindow;
