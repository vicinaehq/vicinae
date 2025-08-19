"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const react_1 = require("react");
const ctx = (0, react_1.createContext)({
    pop: () => { throw new Error('not implemented'); },
    push: () => { throw new Error('not implemented'); },
});
exports.default = ctx;
