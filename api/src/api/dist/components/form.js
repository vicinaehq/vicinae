"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.Form = void 0;
const jsx_runtime_1 = require("src/jsx/jsx-runtime");
const use_imperative_form_handle_1 = require("../hooks/use-imperative-form-handle");
;
;
const FormRoot = ({ enableDrafts = false, actions, children, isLoading = false, navigationTitle, searchBarAccessory }) => {
    return ((0, jsx_runtime_1.jsxs)("root-form", { enableDrafts: enableDrafts, isLoading: isLoading, navigationTitle: navigationTitle, children: [searchBarAccessory, children, actions] }));
};
;
;
const TextField = ({ ref }) => {
    (0, use_imperative_form_handle_1.useImperativeFormHandle)(ref);
    return (0, jsx_runtime_1.jsx)("text-field", {});
};
;
const PasswordField = () => {
    return (0, jsx_runtime_1.jsx)("password-field", {});
};
exports.Form = Object.assign(FormRoot, {
    TextField,
    PasswordField,
});
