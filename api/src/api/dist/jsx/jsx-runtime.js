"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.jsx = exports.jsxs = exports.Fragment = void 0;
exports.Fragment = Symbol.for('omnicast.fragment');
// Same implementation, but may be optimized for arrays of children
function jsxs(type, props, key) {
    return jsx(type, props, key);
}
exports.jsxs = jsxs;
function jsx(type, props, key) {
    // Your custom element creation logic
    return {
        $$typeof: Symbol.for('omnicast.element'),
        type,
        key: key === undefined ? null : key,
        props: props || {},
        // Any other properties your renderer needs
    };
}
exports.jsx = jsx;
