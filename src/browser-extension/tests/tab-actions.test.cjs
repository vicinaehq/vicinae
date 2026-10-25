const assert = require("node:assert/strict");
const { readFileSync } = require("node:fs");
const vm = require("node:vm");

const updates = [];
const bookmarks = [];
const sent = [];
const duplicates = [];
const errors = [];
const event = { addListener() {} };
const chrome = {
  action: { setBadgeText() {}, setBadgeBackgroundColor() {} },
  runtime: {
    getManifest: () => ({ version: "1.2.3" }),
    connectNative: () => ({ postMessage: (message) => sent.push(message), onMessage: event, onDisconnect: event }),
    sendMessage: () => Promise.resolve(),
    onMessage: event,
  },
  tabs: {
    update: (id, change, cb) => { updates.push({ id, change }); cb(); },
    duplicate: (id, cb) => { duplicates.push(id); cb(); },
    onCreated: event, onUpdated: event, onRemoved: event, onActivated: event, onMoved: event,
  },
  bookmarks: {
    create: (bookmark, cb) => { bookmarks.push(bookmark); cb(); },
    getTree: (cb) => cb([{ id: "0", children: [
      { id: "1", title: "Bookmarks bar", children: [
        { id: "2", title: "Empty", children: [] },
        { id: "3", title: "News", children: [{ id: "4", title: "Article", url: "https://example.com" }] },
      ] },
      { id: "5", title: "Other bookmarks", children: [] },
    ] }]),
  },
  permissions: { getAll: (callback) => callback({ permissions: [] }), onAdded: event, onRemoved: event },
  storage: { local: { get: async () => ({}), set: async () => {} } },
};
const context = vm.createContext({ chrome, crypto: { randomUUID: () => "test" },
  navigator: { userAgent: "test" }, console: { log() {}, error: (error) => errors.push(error) }, clearTimeout });
vm.runInContext(readFileSync(require.resolve("../chrome/background.js"), "utf8"), context);

context.handleNotification("browser/set-pinned", { tabId: 12, pinned: true });
context.handleNotification("browser/set-pinned", { tabId: 12, pinned: false });
context.handleNotification("browser/set-muted", { tabId: 12, muted: true });
context.handleNotification("browser/set-muted", { tabId: 12, muted: false });
context.handleNotification("browser/bookmark-tab", { tabId: 12, title: "Edited", url: "https://edited.example" });
context.handleNotification("browser/bookmark-tab", { tabId: 12, title: "News", url: "https://example.com", folderId: "3" });
context.handleNotification("browser/duplicate-tab", { tabId: 12 });

assert.deepEqual(JSON.parse(JSON.stringify(updates)), [
  { id: 12, change: { pinned: true } },
  { id: 12, change: { pinned: false } },
  { id: 12, change: { muted: true } },
  { id: 12, change: { muted: false } },
]);
assert.deepEqual(JSON.parse(JSON.stringify(bookmarks)), [
  { title: "Edited", url: "https://edited.example" },
  { title: "News", url: "https://example.com", parentId: "3" },
]);
assert.deepEqual(duplicates, [12]);
assert.deepEqual(errors, []);

setImmediate(() => {
  context.sendBookmarks(42);
  const response = sent.find((message) => message.method === "browser/bookmarks");
  assert.equal(response.data.requestId, 42);
  assert.deepEqual(JSON.parse(JSON.stringify(response.data.folders)), [
    { id: "1", path: ["Bookmarks bar"] },
    { id: "2", path: ["Bookmarks bar", "Empty"] },
    { id: "3", path: ["Bookmarks bar", "News"] },
    { id: "5", path: ["Other bookmarks"] },
  ]);
});
