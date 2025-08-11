"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.bus = void 0;
const crypto_1 = require("crypto");
const worker_threads_1 = require("worker_threads");
class Bus {
    port;
    requestMap = new Map();
    listeners = [];
    handleMessage(message) {
        const { envelope, data } = message;
        console.log('request map size is ', this.requestMap.size);
        console.log('got message', { envelope });
        if (envelope.type == 'response') {
            const request = this.requestMap.get(envelope.id);
            if (!request) {
                console.error(`Received response for unknown request ${envelope.action} ${envelope.id}`);
                return;
            }
            this.requestMap.delete(envelope.id);
            request.resolve(message);
            console.log({ resolve: message });
            return;
        }
        if (envelope.type == 'event') {
            console.log('got event in extension', envelope);
            for (const listener of this.listeners) {
                if (listener.type != envelope.action)
                    continue;
                listener.handler(...(data.args ?? []));
            }
            return;
        }
        if (envelope.type == 'request') {
            console.error(`Direct requests to extensions are not yet supported`);
            return;
        }
        console.log('resolved request', message);
    }
    constructor(port) {
        this.port = port;
        if (!port)
            return;
        port.on('message', this.handleMessage.bind(this));
        port.on('messageerror', (error) => {
            console.error(`Message error from manager`, error);
        });
        port.on('close', () => {
            console.error(`Parent port closed prematurely`);
        });
    }
    unsubscribe(id) {
        const idx = this.listeners.findIndex(lstn => lstn.id == id);
        if (idx == -1)
            return;
        this.listeners.splice(idx, 1);
    }
    subscribe(type, cb) {
        const item = { id: (0, crypto_1.randomUUID)(), type, handler: cb };
        this.listeners.push(item);
        return {
            unsubscribe: () => this.unsubscribe(item.id)
        };
    }
    emit(action, data) {
        const id = (0, crypto_1.randomUUID)();
        const message = {
            envelope: {
                type: 'event',
                action,
                id
            },
            data
        };
        this.port.postMessage(message);
    }
    request(action, data = {}) {
        const id = (0, crypto_1.randomUUID)();
        return new Promise((resolve, reject) => {
            const resolver = (message) => resolve(message);
            try {
                this.requestMap.set(id, { resolve: resolver });
                console.log(`add id for message type ${action}` + id);
                const message = {
                    envelope: {
                        type: 'request',
                        action,
                        id
                    },
                    data
                };
                this.port.postMessage(message);
            }
            catch (error) {
                reject(error);
            }
        });
    }
}
;
exports.bus = new Bus(worker_threads_1.parentPort);
