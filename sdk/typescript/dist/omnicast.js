"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const net_1 = require("net");
class Client {
    constructor() {
        const client = (0, net_1.connect)({ path: process.env.ENDPOINT });
    }
}
;
