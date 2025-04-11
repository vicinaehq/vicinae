"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.useApplications = void 0;
const react_1 = require("react");
const utils_1 = require("../utils");
const useApplications = () => {
    const [isLoading, setIsLoading] = (0, react_1.useState)(true);
    const [error, setError] = (0, react_1.useState)(null);
    const [apps, setApps] = (0, react_1.useState)([]);
    (0, react_1.useEffect)(() => {
        setIsLoading(true);
        (0, utils_1.getApplications)()
            .then(setApps)
            .catch(setError)
            .finally(() => setIsLoading(false));
    }, []);
    return [apps, isLoading, error];
};
exports.useApplications = useApplications;
