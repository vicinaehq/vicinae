const assert = require("node:assert/strict");
const { readFileSync } = require("node:fs");
const vm = require("node:vm");

const updates = [];
const bookmarks = [];
const duplicates = [];
const errors = [];
const tab = { id: 12, title: "Example", url: "https://example.com" };
const event = { addListener() {} };
const chrome = {
  action: { setBadgeText() {}, setBadgeBackgroundColor() {} },
  runtime: {
    connectNative: () => ({ postMessage() {}, onMessage: event, onDisconnect: event }),
    sendMessage: () => Promise.resolve(),
    onMessage: event,
  },
  tabs: {
    get: (id, cb) => cb(id === tab.id ? tab : undefined),
    update: (id, change, cb) => { updates.push({ id, change }); cb(); },
    duplicate: (id, cb) => { duplicates.push(id); cb(); },
    onCreated: event, onUpdated: event, onRemoved: event, onActivated: event, onMoved: event,
  },
  bookmarks: { create: (bookmark, cb) => { bookmarks.push(bookmark); cb(); } },
};
const context = vm.createContext({ chrome, crypto: { randomUUID: () => "test" },
  navigator: { userAgent: "test" }, console: { log() {}, error: (error) => errors.push(error) }, clearTimeout });
vm.runInContext(readFileSync(require.resolve("../chrome/background.js"), "utf8"), context);

context.handleNotification("browser/set-pinned", { tabId: 12, pinned: true });
context.handleNotification("browser/set-pinned", { tabId: 12, pinned: false });
context.handleNotification("browser/set-muted", { tabId: 12, muted: true });
context.handleNotification("browser/set-muted", { tabId: 12, muted: false });
context.handleNotification("browser/bookmark-tab", { tabId: 12 });
context.handleNotification("browser/bookmark-tab", { tabId: 999 });
context.handleNotification("browser/duplicate-tab", { tabId: 12 });

assert.deepEqual(JSON.parse(JSON.stringify(updates)), [
  { id: 12, change: { pinned: true } },
  { id: 12, change: { pinned: false } },
  { id: 12, change: { muted: true } },
  { id: 12, change: { muted: false } },
]);
assert.deepEqual(JSON.parse(JSON.stringify(bookmarks)), [{ title: "Example", url: "https://example.com" }]);
assert.deepEqual(duplicates, [12]);
assert.deepEqual(errors, ["Tab has no URL to bookmark"]);
