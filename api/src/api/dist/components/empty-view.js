"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.EmptyView = void 0;
const jsx_runtime_1 = require("src/jsx/jsx-runtime");
const image_1 = require("../image");
const EmptyView = ({ icon, actions, ...props }) => {
    const nativeProps = props;
    if (icon)
        nativeProps.icon = (0, image_1.serializeImageLike)(icon);
    return ((0, jsx_runtime_1.jsx)("empty-view", { ...nativeProps, children: actions }));
};
exports.EmptyView = EmptyView;
