"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.LocalImage = exports.Image = void 0;
const types_1 = require("./types");
class Image extends types_1.NativeElement {
    constructor(props = {}) {
        super(Image.name, props, []);
    }
    size(n) {
        return this.width(n).height(n);
    }
    width(n) {
        return this.buildProp('width', n);
    }
    height(n) {
        return this.buildProp('height', n);
    }
}
exports.Image = Image;
;
class LocalImage extends Image {
    constructor(path) {
        super({ path });
    }
}
exports.LocalImage = LocalImage;
