"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.HStack = exports.VStack = exports.Container = exports.Alignments = void 0;
const types_1 = require("./types");
exports.Alignments = ['left', 'top', 'bottom', 'right', 'center', 'hcenter', 'vcenter'];
class Container extends types_1.NativeElement {
    constructor(children, props) {
        super('container', props, children);
    }
    margins(n) {
        return this.buildProp('margins', n);
    }
    style(s) { return this.buildProp('style', s); }
    spacing(n) { return this.buildProp('spacing', n); }
    align(align) { return this.buildProp('align', align); }
}
exports.Container = Container;
class VStack extends Container {
    constructor(...children) {
        super(children.filter((s) => !!s), { margins: 0, direction: 'vertical' });
    }
}
exports.VStack = VStack;
class HStack extends Container {
    constructor(...children) {
        super(children.filter((s) => !!s), { margins: 0, direction: 'horizontal' });
    }
}
exports.HStack = HStack;
