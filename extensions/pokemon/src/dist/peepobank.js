"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const jsx_runtime_1 = require("react/jsx-runtime");
const api_1 = require("@omnicast/api");
const react_1 = require("react");
const Command = () => {
    (0, react_1.useEffect)(() => {
        console.log('create peepobank');
        const handle = setInterval(() => {
            console.log('tick');
        }, 1000);
        return () => clearInterval(handle);
    }, []);
    return ((0, jsx_runtime_1.jsxs)(api_1.List, { onSearchTextChange: (query) => console.log(query), children: [(0, jsx_runtime_1.jsx)(api_1.List.Item, { title: "Item 1" }), (0, jsx_runtime_1.jsx)(api_1.List.Item, { title: "Item 2" }), (0, jsx_runtime_1.jsx)(api_1.List.Item, { title: "Item 3" }), (0, jsx_runtime_1.jsx)(api_1.List.Item, { title: "Item 4" })] }));
};
exports.default = Command;
