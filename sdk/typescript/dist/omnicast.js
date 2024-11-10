"use strict";
var __awaiter = (this && this.__awaiter) || function (thisArg, _arguments, P, generator) {
    function adopt(value) { return value instanceof P ? value : new P(function (resolve) { resolve(value); }); }
    return new (P || (P = Promise))(function (resolve, reject) {
        function fulfilled(value) { try { step(generator.next(value)); } catch (e) { reject(e); } }
        function rejected(value) { try { step(generator["throw"](value)); } catch (e) { reject(e); } }
        function step(result) { result.done ? resolve(result.value) : adopt(result.value).then(fulfilled, rejected); }
        step((generator = generator.apply(thisArg, _arguments || [])).next());
    });
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.OmnicastClient = exports.MessageTypes = void 0;
const net_1 = require("net");
const native_ui_1 = require("./native-ui");
const crypto_1 = require("crypto");
const util_1 = require("util");
exports.MessageTypes = [
    'render',
    'echo',
    'register',
    'event',
];
class OmnicastClient {
    findHandler(node, type) {
        var _a, _b;
        return (_b = (_a = this.handlerStore.get(node.id)) === null || _a === void 0 ? void 0 : _a.get(type)) !== null && _b !== void 0 ? _b : null;
    }
    createHandler(node, type) {
        var _a;
        const handlers = (_a = this.handlerStore.get(node.id)) !== null && _a !== void 0 ? _a : new Map();
        const id = (0, crypto_1.randomUUID)();
        handlers.set(type, id);
        this.handlerStore.set(node.id, handlers);
        return id;
    }
    getOrCreateHandler(node, type) {
        const id = this.findHandler(node, type);
        if (id)
            return id;
        return this.createHandler(node, type);
    }
    sendRaw(data) {
        const msg = Buffer.from(JSON.stringify(data), 'utf-8');
        const buffer = Buffer.allocUnsafe(msg.length + 4);
        buffer.writeUint32LE(msg.length);
        msg.copy(buffer, 4, 0, msg.length);
        this.socket.write(buffer);
    }
    parseMessage(data) {
        const mlen = data.readUInt32LE();
        const payload = data.subarray(4, mlen + 4);
        if (mlen != payload.length)
            console.warn(`mlen (${mlen}) != payload.length (${payload.length})`);
        const json = JSON.parse(payload.toString('utf-8'));
        return {
            type: json.type,
            data: json.data
        };
    }
    sendMessage(type, payload) {
        this.sendRaw({
            type,
            data: payload
        });
    }
    dispatchAsync(fn) {
        return __awaiter(this, void 0, void 0, function* () {
            const action = yield fn();
            this.app.update(action, { dispatchAsync: this.dispatchAsync.bind(this) });
            this.app.render();
        });
    }
    serializeNativeElement(node) {
        const tree = { component: node, children: [] };
        for (const children of node.children) {
            tree.children.push(this.serializeNode(children));
        }
        return tree;
    }
    serializeNode(node) {
        if (node instanceof native_ui_1.Component) {
            return { component: node, children: [this.serializeNativeElement(node.render())] };
        }
        return this.serializeNativeElement(node);
    }
    printAsHtml(tree) {
        let s = '';
        const recurse = (tree, depth = 0) => {
            const indent = `\t`.repeat(depth);
            const props = Object.entries(tree.component.props).filter(([k]) => !(0, native_ui_1.isHandlerType)(k)).map(([k, v]) => `${k}=${v}`).join(' ');
            if (tree.children.length == 0) {
                s += indent + `<${tree.component.type} ${props} />\n`;
                return;
            }
            s += indent + `<${tree.component.type} ${props}>\n`;
            for (const child of tree.children) {
                recurse(child, depth + 1);
            }
            s += indent + `</${tree.component.type}>\n`;
        };
        recurse(tree);
        console.log(s);
    }
    printTree(node) {
        const serialized = this.serializeNode(node);
        console.log((0, util_1.inspect)(serialized, { colors: true, depth: null }));
    }
    updateComponentTree(root, next) {
        if (!root) {
            return this.updateComponentTree(next, next);
        }
        if (root.component.type != next.component.type) {
            console.log('recreate!');
            root = next;
        }
        root.component.props = Object.assign({}, next.component.props);
        const min = Math.min(root.children.length, root.children.length);
        for (let i = 0; i != min; ++i) {
            root.children[i] = this.updateComponentTree(root.children[i], next.children[i]);
        }
        for (let i = min; i < next.children.length; ++i) {
            root.children.push(this.updateComponentTree(null, next.children[i]));
        }
        while (root.children.length > next.children.length)
            root.children.pop();
        return root;
    }
    serializeTree(tree) {
        const serialize = (ctx, tree) => {
            if (tree.component instanceof native_ui_1.Component) {
                // custom component always have only one child
                return serialize(tree.component, tree.children[0]);
            }
            const { type, props, } = tree.component;
            const data = {
                type,
                props,
                children: []
            };
            for (const [k, v] of Object.entries(data.props)) {
                if ((0, native_ui_1.isHandlerType)(k)) {
                    const HandlerAction = data.props[k];
                    const id = this.getOrCreateHandler(tree.component, k);
                    data.props[k] = id;
                    this.handlerMap.set(id, {
                        dispatcher: (payload) => ctx.update(new HandlerAction(payload), { dispatchAsync: () => { } })
                    });
                }
                else {
                    data.props[k] = v;
                }
            }
            for (const child of tree.children) {
                data.children.push(serialize(ctx, child));
            }
            return data;
        };
        return serialize(this.app, tree);
    }
    render() {
        if (this.root == null)
            this.app.onMount();
        const root = this.app.render();
        const tree = this.updateComponentTree(this.root, this.serializeNode(root));
        const data = this.serializeTree(tree);
        //this.printTree(root);
        console.log(JSON.stringify(data, null, 2));
        this.root = tree;
        this.sendMessage('render', { root: data });
    }
    constructor(app) {
        this.app = app;
        this.handlerMap = new Map;
        this.handlerStore = new Map;
        this.root = null;
        this.socket = (0, net_1.createConnection)({ path: process.env.ENDPOINT });
        this.sendMessage('register', { token: process.env.TOKEN });
        this.socket.on('data', (data) => {
            try {
                const message = this.parseMessage(data);
                if (message.type == 'event') {
                    console.log(message);
                    const handler = this.handlerMap.get(message.data.handlerId);
                    if (!handler) {
                        console.log('no handler ' + message.data.handlerId);
                        return;
                    }
                    handler.dispatcher(message.data);
                    this.render();
                }
            }
            catch (error) {
                console.error(error);
            }
        });
        this.socket.on('error', (error) => { throw error; });
    }
}
exports.OmnicastClient = OmnicastClient;
