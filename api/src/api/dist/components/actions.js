"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.Action = void 0;
const jsx_runtime_1 = require("src/jsx/jsx-runtime");
const index_1 = require("../hooks/index");
const clipboard_1 = require("../clipboard");
const image_1 = require("../image");
const utils_1 = require("../utils");
const ActionRoot = ({ icon, ...props }) => {
    const nativeProps = props;
    if (icon) {
        nativeProps.icon = (0, image_1.serializeImageLike)(icon);
    }
    return (0, jsx_runtime_1.jsx)("action", { ...nativeProps });
};
const CopyToClipboard = ({ content, ...props }) => {
    return (0, jsx_runtime_1.jsx)(ActionRoot, { ...props, onAction: () => {
            clipboard_1.Clipboard.copy(content);
        } });
};
const Open = ({ target, app, ...props }) => {
    return (0, jsx_runtime_1.jsx)(ActionRoot, { ...props, onAction: () => {
            (0, utils_1.open)(target, app);
        } });
};
const Push = ({ target, ...props }) => {
    const { push } = (0, index_1.useNavigation)();
    return (0, jsx_runtime_1.jsx)(ActionRoot, { ...props, onAction: () => {
            console.log('activate push action');
            push(target);
        } });
};
exports.Action = Object.assign(ActionRoot, {
    CopyToClipboard,
    Push,
    Open
});
