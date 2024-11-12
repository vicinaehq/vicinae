"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.TextChangedAction = exports.KeyPressAction = exports.CurrentRowChangedAction = exports.isHandlerType = exports.HandlerTypes = exports.Component = exports.NativeElement = exports.Action = void 0;
const crypto_1 = require("crypto");
class Action {
}
exports.Action = Action;
;
class NativeElement {
    constructor(type, props, children) {
        this.type = type;
        this.props = props;
        this.children = children;
        this.id = Symbol('');
    }
    buildProp(key, value) {
        this.props[key] = value;
        return this;
    }
    selfAlign(align) {
        this.props.selfAlign = align;
        return this;
    }
    stretch(n) {
        this.props.stretch = n;
        return this;
    }
}
exports.NativeElement = NativeElement;
;
class Component {
    constructor() {
        this.id = Symbol((0, crypto_1.randomUUID)());
        this.type = this.constructor.name;
        this.props = {};
        this.children = [];
    }
}
exports.Component = Component;
;
exports.HandlerTypes = [
    'onTextChanged',
    'currentRowChanged',
    'onKeyPress'
];
const isHandlerType = (s) => exports.HandlerTypes.includes(s);
exports.isHandlerType = isHandlerType;
class CurrentRowChangedAction {
    constructor(init) {
        this.value = init.value;
    }
}
exports.CurrentRowChangedAction = CurrentRowChangedAction;
;
class KeyPressAction {
    constructor(init) {
        this.key = init.value;
    }
}
exports.KeyPressAction = KeyPressAction;
;
class TextChangedAction {
    constructor(init) {
        this.value = init.value;
    }
}
exports.TextChangedAction = TextChangedAction;
;
