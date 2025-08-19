"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.useNavigation = void 0;
const react_1 = require("react");
const navigation_context_1 = __importDefault(require("../context/navigation-context"));
const useNavigation = () => {
    const { push, pop } = (0, react_1.useContext)(navigation_context_1.default);
    return { push, pop };
};
exports.useNavigation = useNavigation;
