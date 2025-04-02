"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.List = void 0;
const jsx_runtime_1 = require("src/jsx/jsx-runtime");
const react_1 = require("react");
const image_1 = require("../image");
const crypto_1 = require("crypto");
const metadata_1 = require("./metadata");
const empty_view_1 = require("./empty-view");
const hooks_1 = require("../hooks");
const ListRoot = ({ onSearchTextChange, onSelectionChange, ...props }) => {
    const searchTextChangeHandler = (0, hooks_1.useEventListener)(onSearchTextChange);
    const selectionChangeHandler = (0, hooks_1.useEventListener)(onSelectionChange);
    return (0, jsx_runtime_1.jsx)("list", { onSearchTextChange: searchTextChangeHandler, onSelectionChange: selectionChangeHandler, ...props });
};
const ListItem = ({ detail, actions, ...props }) => {
    const id = (0, react_1.useRef)(props.id ?? (0, crypto_1.randomUUID)());
    const nativeProps = {
        title: props.title,
        subtitle: props.subtitle,
        id: id.current
    };
    if (props.icon)
        nativeProps.icon = (0, image_1.serializeImageLike)(props.icon);
    return ((0, jsx_runtime_1.jsxs)("list-item", { ...nativeProps, children: [detail, actions] }));
};
const ListItemDetail = ({ metadata, ...props }) => {
    return ((0, jsx_runtime_1.jsx)("list-item-detail", { ...props, children: metadata }));
};
const ListSection = (props) => {
    const nativeProps = props;
    return (0, jsx_runtime_1.jsx)("list-section", { ...nativeProps });
};
exports.List = Object.assign(ListRoot, {
    Section: ListSection,
    EmptyView: empty_view_1.EmptyView,
    Item: Object.assign(ListItem, {
        Detail: Object.assign(ListItemDetail, {
            Metadata: metadata_1.Metadata
        })
    })
});
