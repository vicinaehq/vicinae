"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.useImperativeFormHandle = void 0;
const crypto_1 = require("crypto");
const react_1 = require("react");
const bus_1 = require("../bus");
const useImperativeFormHandle = (ref) => {
    const handleId = (0, react_1.useRef)((0, crypto_1.randomUUID)());
    (0, react_1.useImperativeHandle)(ref, () => {
        return {
            focus: () => {
                bus_1.bus.emit(handleId.current, {
                    type: 'focus'
                });
            },
            reset: () => {
                bus_1.bus.emit(handleId.current, {
                    type: 'reset'
                });
            }
        };
    }, []);
    return [handleId.current];
};
exports.useImperativeFormHandle = useImperativeFormHandle;
