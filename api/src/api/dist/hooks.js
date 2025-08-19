"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.useEventListener = void 0;
const react_1 = require("react");
const bus_1 = require("./bus");
const crypto_1 = require("crypto");
const useEventListener = (fn) => {
    const id = (0, react_1.useRef)((0, crypto_1.randomUUID)());
    (0, react_1.useEffect)(() => {
        if (!fn)
            return;
        bus_1.bus?.subscribe(id.current, fn);
        return () => { bus_1.bus?.unsubscribe(id.current); };
    }, []);
    return fn && id.current;
};
exports.useEventListener = useEventListener;
