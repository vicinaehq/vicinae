"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.ActionPanel = void 0;
const jsx_runtime_1 = require("src/jsx/jsx-runtime");
const image_1 = require("../image");
const ActionPanelRoot = (props) => {
    const nativeProps = props;
    return ((0, jsx_runtime_1.jsx)("action-panel", { ...nativeProps }));
};
const ActionPanelSection = (props) => {
    const nativeProps = props;
    return ((0, jsx_runtime_1.jsx)("action-panel-section", { ...nativeProps }));
};
const ActionPannelSubmenu = ({ icon, ...props }) => {
    const nativeProps = props;
    if (icon)
        nativeProps.icon = (0, image_1.serializeImageLike)(icon);
    return (0, jsx_runtime_1.jsx)("action-panel-submenu", { ...nativeProps });
};
exports.ActionPanel = Object.assign(ActionPanelRoot, {
    Section: ActionPanelSection,
    Submenu: ActionPannelSubmenu,
});
