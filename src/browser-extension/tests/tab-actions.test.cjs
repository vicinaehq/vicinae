const assert = require("node:assert/strict");
const { readFileSync } = require("node:fs");
const vm = require("node:vm");

const updates = [];
const bookmarks = [];
const sent = [];
const duplicates = [];
const errors = [];
const event = { addListener() {} };
let permissionGranted = true;
const chrome = {
  action: { setBadgeText() {}, setBadgeBackgroundColor() {} },
  runtime: {
    getManifest: () => ({ version: "1.2.3" }),
    getURL: (path) => path,
    connectNative: () => ({
      postMessage: (message) => sent.push(message),
      onMessage: event,
      onDisconnect: event,
    }),
    sendMessage: () => Promise.resolve(),
    onMessage: event,
  },
  tabs: {
    query: (_, cb) => cb([]),
    create: (_, cb) => cb({ windowId: 1 }),
    update: (id, change, cb) => {
      updates.push({ id, change });
      cb();
    },
    duplicate: (id, cb) => {
      if (id === 999) chrome.runtime.lastError = { message: "No such tab" };
      else duplicates.push(id);
      cb();
      delete chrome.runtime.lastError;
    },
    onCreated: event,
    onUpdated: event,
    onRemoved: event,
    onActivated: event,
    onMoved: event,
  },
  windows: { update: (_, __, cb) => cb() },
  bookmarks: {
    create: (bookmark, cb) => {
      bookmarks.push(bookmark);
      cb();
    },
    getTree: (cb) =>
      cb([
        {
          id: "0",
          children: [
            {
              id: "1",
              title: "Bookmarks bar",
              children: [
                { id: "2", title: "Empty", children: [] },
                {
                  id: "3",
                  title: "News",
                  children: [
                    { id: "4", title: "Article", url: "https://example.com" },
                  ],
                },
              ],
            },
            { id: "5", title: "Other bookmarks", children: [] },
          ],
        },
      ]),
  },
  history: {
    search: (_, cb) => cb([{ id: "visit", url: "https://example.com" }]),
  },
  permissions: {
    getAll: (cb) => cb({ permissions: ["bookmarks", "history"] }),
    contains: (_, cb) => cb(permissionGranted),
    onAdded: event,
    onRemoved: event,
  },
  storage: { local: { get: async () => ({}), set: async () => {} } },
};
const context = vm.createContext({
  chrome,
  crypto: { randomUUID: () => "test" },
  navigator: { userAgent: "test" },
  console: { log() {}, error: (error) => errors.push(error) },
  clearTimeout,
});
vm.runInContext(
  readFileSync(require.resolve("../chrome/background.js"), "utf8"),
  context,
);

let serial = 0;
async function request(method, data = {}) {
  const id = serial++;
  context.processNativeMessage({ id, method, data });
  await new Promise(setImmediate);
  const response = sent.find(
    (message) =>
      message.id === id && ("result" in message || "error" in message),
  );
  assert.ok(response, `Missing response for ${method}`);
  return JSON.parse(JSON.stringify(response));
}

(async () => {
  await new Promise(setImmediate);
  assert.equal(
    (await request("browser/set-pinned", { tabId: 12, pinned: true })).error,
    undefined,
  );
  await request("browser/set-pinned", { tabId: 12, pinned: false });
  await request("browser/set-muted", { tabId: 12, muted: true });
  await request("browser/set-muted", { tabId: 12, muted: false });
  await request("browser/bookmark-tab", {
    tabId: 12,
    title: "Edited",
    url: "https://edited.example",
  });
  await request("browser/bookmark-tab", {
    tabId: 12,
    title: "News",
    url: "https://example.com",
    folderId: "3",
  });
  await request("browser/duplicate-tab", { tabId: 12 });

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
  assert.deepEqual((await request("browser/get-bookmark-folders")).result, [
    { id: "1", path: ["Bookmarks bar"] },
    { id: "2", path: ["Bookmarks bar", "Empty"] },
    { id: "3", path: ["Bookmarks bar", "News"] },
    { id: "5", path: ["Other bookmarks"] },
  ]);
  const savedBookmarks = (await request("browser/get-bookmarks")).result;
  assert.ok(Array.isArray(savedBookmarks));
  assert.equal(savedBookmarks[0].title, "Article");
  assert.equal(
    (await request("browser/search-history", { query: "", maxResults: 10 }))
      .result[0].id,
    "visit",
  );
  assert.equal(
    (await request("browser/duplicate-tab", { tabId: 999 })).error,
    "No such tab",
  );
  permissionGranted = false;
  assert.equal(
    (
      await request("browser/bookmark-tab", {
        title: "Denied",
        url: "https://example.com",
      })
    ).error,
    "Bookmarks permission not granted",
  );
  assert.equal(bookmarks.length, 2);
  assert.match((await request("browser/unknown")).error, /Unknown method/);
  assert.deepEqual(errors, []);
})().catch((error) => {
  console.error(error);
  process.exitCode = 1;
});
