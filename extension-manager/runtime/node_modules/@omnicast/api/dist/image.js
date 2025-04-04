"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.serializeImageLike = exports.Image = void 0;
var Image;
(function (Image) {
    let Mask;
    (function (Mask) {
        Mask["Circle"] = "circle";
        Mask["RoundedRectangle"] = "roundedRectangle";
    })(Mask = Image.Mask || (Image.Mask = {}));
    ;
})(Image || (exports.Image = Image = {}));
;
const serializeImageLike = (image) => {
    if (image instanceof URL) {
        return { source: image.toString() };
    }
    if (typeof image == "string") {
        return { source: image };
    }
    return image;
};
exports.serializeImageLike = serializeImageLike;
