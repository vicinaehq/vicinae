const assert = require("node:assert/strict");
const { readFileSync } = require("node:fs");
const vm = require("node:vm");

const script = readFileSync(require.resolve("../chrome/background.js"), "utf8");

async function start(saved, browserInfo) {
  const sent = [];
  let onMessage;
  let onPopupMessage;
  const event = { addListener() {} };
  const storage = {
    get: async () => saved,
    set: async (value) => Object.assign(saved, value),
  };
  const chrome = {
    storage: { local: storage },
    action: { setBadgeText() {}, setBadgeBackgroundColor() {} },
    runtime: {
      getManifest: () => ({ version: "1.2.3" }),
      connectNative: () => ({
        postMessage: (message) => sent.push(message),
        onMessage: { addListener: (listener) => { onMessage = listener; } },
        onDisconnect: event,
      }),
      sendMessage: async () => {},
      onMessage: { addListener: (listener) => { onPopupMessage = listener; } },
    },
    permissions: { getAll: (callback) => callback({ permissions: ["bookmarks"] }),
      onAdded: event, onRemoved: event },
    tabs: { query: (_, callback) => callback([]),
      onCreated: event, onUpdated: event, onRemoved: event, onActivated: event, onMoved: event },
  };
  const context = vm.createContext({ chrome, browser: browserInfo && {
    runtime: { getBrowserInfo: async () => browserInfo }, storage: { local: storage },
  }, crypto: { randomUUID: () => "stable-id" },
  navigator: { userAgent: "Mozilla/5.0 Chrome/100", userAgentData: { brands: [{ brand: "Google Chrome" }] } },
  console: { log() {}, error() {} }, clearTimeout });
  vm.runInContext(script, context);
  await new Promise(setImmediate);
  return { sent, onMessage: (message) => onMessage(message),
    onPopupMessage: (message, reply) => onPopupMessage(message, {}, reply) };
}

(async () => {
  const saved = {};
  const first = await start(saved);
  assert.equal(saved.browserId, "stable-id");
  assert.deepEqual(JSON.parse(JSON.stringify(first.sent[0].data)), {
    id: "stable-id", name: "Google Chrome", engine: "chromium",
    version: "1.2.3", permissions: ["bookmarks"],
  });
  first.onMessage({ id: first.sent[0].id, result: {} });
  let response;
  first.onPopupMessage({ type: "profile_name_changed", name: "Work" }, (value) => { response = value; });
  assert.equal(response.success, true);
  await new Promise(setImmediate);
  assert.equal(first.sent.at(-1).data.name, "Google Chrome · Work");
  first.onMessage({ id: first.sent.at(-1).id, result: {} });
  assert.equal(first.sent.at(-1).method, "browser/tabs-changed");

  saved.profileName = "Work";
  const restarted = await start(saved);
  assert.equal(restarted.sent[0].data.id, "stable-id");
  assert.equal(restarted.sent[0].data.name, "Google Chrome · Work");

  const firefox = await start({}, { name: "Firefox", vendor: "Mozilla" });
  assert.equal(firefox.sent[0].data.name, "Firefox");
  assert.equal(firefox.sent[0].data.engine, "gecko");
})().catch((error) => { console.error(error); process.exitCode = 1; });
