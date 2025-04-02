"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.showToast = void 0;
const bus_1 = require("./bus");
/*
export const showToast = (options: Toast.Options): Promise<Toast> => {
    return {} as any;
}
*/
const showToast = async (init, title, message) => {
    // we are dealing with the style (preferred) overload
    if (typeof init === "string") {
    }
    else {
        const opts = init;
    }
    const toast = await bus_1.bus?.request("show-toast", {});
    return {};
};
exports.showToast = showToast;
