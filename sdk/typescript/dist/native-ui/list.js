"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.ListItem = exports.List = exports.ScrollbarPolicies = void 0;
const types_1 = require("./types");
exports.ScrollbarPolicies = [
    'always-on',
    'always-off',
    'auto'
];
class List extends types_1.NativeElement {
    constructor(...children) {
        super(List.name, {}, children);
    }
    selected(n) { return this.buildProp('selected', n); }
    stretch(n) { return this.buildProp('stretch', n); }
    horizontalScrollBar(s) { return this.buildProp('hscroll', s); }
    verticalScrollBar(s) { return this.buildProp('vscroll', s); }
    currentRowChanged(action) { return this.buildProp('currentRowChanged', action); }
}
exports.List = List;
class ListItem extends types_1.NativeElement {
    constructor(name) {
        if (typeof name == 'string') {
            super(ListItem.name, { label: name }, []);
            return;
        }
        super(ListItem.name, {}, [name]);
    }
}
exports.ListItem = ListItem;
