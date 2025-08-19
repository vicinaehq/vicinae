"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.Metadata = void 0;
const jsx_runtime_1 = require("src/jsx/jsx-runtime");
const image_1 = require("../image");
const tag_1 = require("./tag");
const MetadataRoot = (props) => {
    return (0, jsx_runtime_1.jsx)("metadata", { ...props });
};
const MetadataLabel = ({ icon, ...props }) => {
    const nativeProps = props;
    if (icon)
        nativeProps.icon = (0, image_1.serializeImageLike)(icon);
    return (0, jsx_runtime_1.jsx)("metadata-label", { ...props });
};
const MetadataSeparator = () => {
    return (0, jsx_runtime_1.jsx)("metadata-separator", {});
};
exports.Metadata = Object.assign(MetadataRoot, {
    Label: MetadataLabel,
    Separator: MetadataSeparator,
    TagList: tag_1.TagList
});
