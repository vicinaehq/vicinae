"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.TagList = void 0;
const jsx_runtime_1 = require("src/jsx/jsx-runtime");
const image_1 = require("../image");
const color_1 = require("../color");
const TagListRoot = ({ title, children }) => {
    const nativeProps = {
        title,
        children
    };
    return (0, jsx_runtime_1.jsx)("tag-list", { ...nativeProps });
};
const TagItem = ({ color, icon, text, onAction }) => {
    const nativeProps = {
        text, onAction
    };
    if (color)
        nativeProps.color = (0, color_1.serializeColorLike)(color);
    if (icon)
        nativeProps.icon = (0, image_1.serializeImageLike)(icon);
    return (0, jsx_runtime_1.jsx)("tag-item", { ...nativeProps });
};
exports.TagList = Object.assign(TagListRoot, {
    Item: TagItem
});
