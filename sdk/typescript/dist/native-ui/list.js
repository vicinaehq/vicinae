"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.ListItem = exports.List = void 0;
const types_1 = require("./types");
class List extends types_1.NativeElement {
    constructor(...children) {
        super(List.name, {}, children);
    }
    selected(n) {
        return this.buildProp('selected', n);
    }
    currentRowChanged(action) {
        return this.buildProp('currentRowChanged', action);
    }
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
