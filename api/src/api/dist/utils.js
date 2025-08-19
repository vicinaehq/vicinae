"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.showInFinder = exports.getDefaultApplication = exports.getApplications = exports.getFrontmostApplication = exports.open = exports.trash = exports.captureException = void 0;
const promises_1 = require("fs/promises");
const bus_1 = require("./bus");
const captureException = (exception) => {
    // maybe one day, if we have a developer hub, also...
    console.error("captureException called on", exception);
};
exports.captureException = captureException;
// Linux systems usually do not have a trash, but maybe we should support one...
const trash = async (path) => {
    const targets = Array.isArray(path) ? path : [path];
    const promises = targets.map((p) => (0, promises_1.rm)(p, { recursive: true }));
    await Promise.all(promises);
};
exports.trash = trash;
const open = (target, app) => {
    let appId;
    if (app) {
        if (typeof app === 'string') {
            appId = app;
        }
        else {
            appId = app.id;
        }
    }
    bus_1.bus?.request('open-target', {
        target,
        appId
    });
};
exports.open = open;
const deserializeApp = (app) => ({ id: app.id, name: app.name, icon: { iconName: app.icon } });
const getFrontmostApplication = async () => {
    const res = await bus_1.bus.request('apps.get-frontmost');
    if (!res.data.app) {
        throw new Error('couldnt get frontmost app');
    }
    return deserializeApp(res.data.app);
};
exports.getFrontmostApplication = getFrontmostApplication;
const getApplications = async (path) => {
    const res = await bus_1.bus.request('apps.get', {
        target: path?.toString()
    });
    return res.data.apps.map(deserializeApp);
};
exports.getApplications = getApplications;
const getDefaultApplication = async (path) => {
    const res = await bus_1.bus.request('apps.get-default', {
        target: path.toString()
    });
    if (!res.data.app) {
        throw new Error('couldnt get default app');
    }
    return deserializeApp(res.data.app);
};
exports.getDefaultApplication = getDefaultApplication;
const showInFinder = async (path) => {
    await bus_1.bus.request('show-in-finder');
};
exports.showInFinder = showInFinder;
