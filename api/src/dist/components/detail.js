"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.Detail = void 0;
const jsx_runtime_1 = require("src/jsx/jsx-runtime");
const metadata_1 = require("./metadata");
const DetailRoot = ({ metadata, ...props }) => {
    const nativeProps = props;
    return ((0, jsx_runtime_1.jsx)("detail", { ...nativeProps, children: metadata }));
};
exports.Detail = Object.assign(DetailRoot, {
    Metadata: metadata_1.Metadata
});
