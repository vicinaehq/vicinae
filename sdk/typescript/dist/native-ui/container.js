"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.HStack = exports.VStack = exports.Container = void 0;
const types_1 = require("./types");
class Container extends types_1.NativeElement {
    constructor(children, props) {
        super('container', props, children);
    }
    margins(n) {
        return this.buildProp('margins', n);
    }
}
exports.Container = Container;
class VStack extends Container {
    constructor(...children) {
        super(children, { margins: 0, direction: 'vertical' });
    }
}
exports.VStack = VStack;
class HStack extends Container {
    constructor(...children) {
        super(children, { margins: 0, direction: 'horizontal' });
    }
}
exports.HStack = HStack;
