"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.NavigationProvider = void 0;
const jsx_runtime_1 = require("src/jsx/jsx-runtime");
const react_1 = require("react");
const navigation_context_1 = __importDefault(require("./navigation-context"));
const bus_1 = require("../bus");
const View = ({ children }) => {
    return (0, jsx_runtime_1.jsx)(jsx_runtime_1.Fragment, { children: children });
};
const NavigationProvider = ({ root }) => {
    const [navStack, setNavStack] = (0, react_1.useState)([]);
    const pop = () => {
        bus_1.bus.request('pop-view', {}).then(() => {
            setNavStack((cur) => cur.slice(0, -1));
        });
    };
    const push = (node) => {
        bus_1.bus.request('push-view', {}).then(() => {
            setNavStack((cur) => [...cur, node]);
        });
    };
    (0, react_1.useEffect)(() => {
        console.log('stack is now of size' + navStack.length);
    }, [navStack]);
    (0, react_1.useEffect)(() => {
        const listener = bus_1.bus.subscribe('pop-view', () => {
            console.log('popping from current nav stack of size ' + navStack.length);
            setNavStack((cur) => cur.slice(0, -1));
        });
        push(root);
        return () => listener.unsubscribe();
    }, []);
    console.log('stack is now of size' + navStack.length);
    return ((0, jsx_runtime_1.jsx)(navigation_context_1.default.Provider, { value: {
            push,
            pop
        }, children: navStack.map((el, idx) => (0, jsx_runtime_1.jsx)(View, { children: el }, idx)) }));
};
exports.NavigationProvider = NavigationProvider;
