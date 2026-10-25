const HOST_NAME = "com.vicinae.vicinae";
const RETRY_INITIAL_MS = 1000;
const RETRY_MAX_MS = 60000;
const TAB_DEBOUNCE_MS = 50;
const OPTIONAL_PERMISSIONS = ["bookmarks", "history"];
const IGNORED_BOOKMARK_SCHEMES = ["place:", "javascript:"];
// 2: vicinae calls into the extension through requests instead of notifications; required by vicinae
const PROTOCOL_VERSION = 2;
const requestMap = new Map();

let browserId;
let browserName;
let browserEngine;
let profileName = "";

let port = null;
let connectionState = "disconnected"; // disconnected, connecting, connected, error
let retryId;
let retryDelay = RETRY_INITIAL_MS;
let serial = 0;
let manuallyDisconnected = false;
let tabDebounceTimer = null;

function setConnectionState(state, error = null) {
  connectionState = state;

  // Update badge
  if (state === "connected") {
    chrome.action.setBadgeText({ text: "" });
  } else if (state === "error") {
    chrome.action.setBadgeBackgroundColor({ color: "#ef4444" });
    chrome.action.setBadgeText({ text: " " });
  } else {
    chrome.action.setBadgeBackgroundColor({ color: "#f59e0b" });
    chrome.action.setBadgeText({ text: " " });
  }

  // Broadcast to popup
  chrome.runtime
    .sendMessage({
      source: "connection_state",
      state,
      manuallyDisconnected,
      ...(error && { error }),
    })
    .catch(() => {});
}

function requestToNative(type, data, cb) {
  const id = serial++;

  requestMap.set(id, cb);
  sendToNativeHost({
    id,
    method: type,
    data,
  });
}

function sendToNativeHost(message) {
  if (!port) {
    console.error(`sendToNativeHost: port doesn't yet exist`);
    return;
  }

  console.log("Sending to native host:", message);
  port.postMessage(message);
}

function notifyNative(type, data) {
  sendToNativeHost({
    method: type,
    data,
  });
}

function replyToNative(id, result, error) {
  sendToNativeHost(
    error === undefined ? { id, result: result ?? null } : { id, error },
  );
}

const REQUEST_HANDLERS = {
  "browser/focus-tab": ({ tabId }) => focusTab(tabId),
  "browser/close-tab": ({ tabId }) => closeTab(tabId),
  "browser/set-pinned": ({ tabId, pinned }) =>
    chromeCall((cb) => chrome.tabs.update(tabId, { pinned }, cb)),
  "browser/set-muted": ({ tabId, muted }) =>
    chromeCall((cb) => chrome.tabs.update(tabId, { muted }, cb)),
  "browser/duplicate-tab": ({ tabId }) =>
    chromeCall((cb) => chrome.tabs.duplicate(tabId, cb)).then(() => {}),
  "browser/bookmark-tab": ({ title, url }) => bookmarkTab(title, url),
  "browser/get-bookmarks": () => getBookmarks(),
  "browser/remove-bookmark": ({ bookmarkId }) => removeBookmark(bookmarkId),
  "browser/search-history": ({ query, maxResults }) =>
    searchHistory(query, maxResults),
  "browser/remove-history-url": ({ url }) => removeHistoryUrl(url),
  "browser/clear-history": () => clearHistory(),
  "browser/request-permission": ({ permission }) =>
    openPermissionPage(permission),
};

function handleRequest(id, type, data) {
  const handler = REQUEST_HANDLERS[type];

  if (!handler) {
    replyToNative(id, undefined, `Unknown method ${type}`);
    return;
  }

  Promise.resolve()
    .then(() => handler(data ?? {}))
    .then(
      (result) => replyToNative(id, result),
      (error) => replyToNative(id, undefined, error?.message ?? String(error)),
    );
}

function handleResponse(id, result, error) {
  const handler = requestMap.get(id);

  if (!handler) {
    console.error(`No handler for request with id ${id}`);
    return;
  }

  requestMap.delete(id);
  handler(result, error);
}

function processNativeMessage(message) {
  console.log("got message:", message);

  if (message.method) {
    if (message.id === undefined) {
      console.error(`Unexpected notification ${message.method}`);
      return;
    }
    return handleRequest(message.id, message.method, message.data);
  }

  return handleResponse(message.id, message.result, message.error);
}

async function detectBrowser() {
  if (typeof browser !== "undefined" && browser.runtime?.getBrowserInfo) {
    try {
      return {
        name: (await browser.runtime.getBrowserInfo()).name,
        engine: "gecko",
      };
    } catch (error) {
      console.error("Could not identify browser:", error);
    }
  }

  try {
    if (await navigator.brave?.isBrave?.())
      return { name: "Brave", engine: "chromium" };
  } catch (error) {
    console.error("Could not identify Brave:", error);
  }
  const brands =
    navigator.userAgentData?.brands?.map(({ brand }) => brand) ?? [];
  if (brands.includes("Microsoft Edge") || /Edg\//.test(navigator.userAgent))
    return { name: "Microsoft Edge", engine: "chromium" };
  if (/OPR\//.test(navigator.userAgent))
    return { name: "Opera", engine: "chromium" };
  if (/Vivaldi\//.test(navigator.userAgent))
    return { name: "Vivaldi", engine: "chromium" };
  if (brands.includes("Google Chrome"))
    return { name: "Google Chrome", engine: "chromium" };
  if (/Firefox\//.test(navigator.userAgent))
    return { name: "Firefox", engine: "gecko" };
  return { name: "Chromium", engine: "chromium" };
}

function sendBrowserInit(onConnected = false) {
  getGrantedPermissions().then((permissions) => {
    requestToNative(
      "browser/init",
      {
        id: browserId,
        name: profileName ? `${browserName} · ${profileName}` : browserName,
        engine: browserEngine,
        version: chrome.runtime.getManifest().version,
        permissions,
        optionalPermissions: OPTIONAL_PERMISSIONS,
        protocolVersion: PROTOCOL_VERSION,
      },
      (_, error) => {
        if (error) {
          console.error("browserInit failed:", error);
          if (onConnected) setConnectionState("error", String(error));
          return;
        }
        if (onConnected) {
          retryDelay = RETRY_INITIAL_MS;
          setConnectionState("connected");
        }
        sendCurrentTabs();
      },
    );
  });
}

// Connect to native host on startup
function connectToNativeHost() {
  console.log("Connecting to native host:", HOST_NAME);
  setConnectionState("connecting");

  clearTimeout(retryId);

  try {
    port = chrome.runtime.connectNative(HOST_NAME);
    requestMap.clear();
    serial = 0;

    sendBrowserInit(true);

    // Handle messages from native host
    port.onMessage.addListener((message) => {
      processNativeMessage(message);
    });

    // Handle disconnect
    port.onDisconnect.addListener(() => {
      console.log("Disconnected from native host");
      const error = chrome.runtime.lastError;

      if (!manuallyDisconnected) {
        retryId = setTimeout(() => {
          connectToNativeHost();
        }, retryDelay);
        retryDelay = Math.min(retryDelay * 2, RETRY_MAX_MS);
      }

      if (error) {
        console.error("Disconnect error:", error.message);
        setConnectionState("error", error.message);
      } else {
        setConnectionState("disconnected");
      }

      port = null;
    });

    return port;
  } catch (error) {
    console.error("Failed to connect:", error);
    setConnectionState("error", error.message);
    return null;
  }
}

// Listen for messages from extension contexts (popup, content scripts)
chrome.runtime.onMessage.addListener((message, sender, sendResponse) => {
  if (message.target === "native_host") {
    sendToNativeHost(message.data);
    sendResponse({ success: true });
  } else if (message.type === "get_connection_state") {
    sendResponse({ state: connectionState, manuallyDisconnected });
  } else if (message.type === "retry_connection") {
    if (connectionState !== "connected" && browserId) {
      manuallyDisconnected = false;
      retryDelay = RETRY_INITIAL_MS;
      connectToNativeHost();
    }
  } else if (message.type === "disconnect_connection") {
    manuallyDisconnected = true;
    clearTimeout(retryId);
    if (port) {
      port.disconnect();
      port = null;
    }
    setConnectionState("disconnected");
  } else if (message.type === "profile_name_changed") {
    if (typeof message.name !== "string") {
      sendResponse({ success: false });
    } else {
      profileName = message.name.trim().slice(0, 60);
      if (port) sendBrowserInit();
      sendResponse({ success: true });
    }
  } else if (message.type === "get_extension_id") {
    sendResponse({ extensionId: chrome.runtime.id });
  }
  return true;
});

function sendCurrentTabs() {
  chrome.tabs.query({}, (tabs) => {
    notifyNative(
      "browser/tabs-changed",
      tabs.map((tab) => ({
        id: tab.id,
        title: tab.title,
        url: tab.url,
        windowId: tab.windowId,
        active: tab.active,
        audible: tab.audible ?? false,
        muted: tab.mutedInfo?.muted ?? false,
        lastAccessed: tab.lastAccessed ?? 0,
        pinned: tab.pinned ?? false,
      })),
    );
  });
}

function sendCurrentTabsDebounced() {
  clearTimeout(tabDebounceTimer);
  tabDebounceTimer = setTimeout(sendCurrentTabs, TAB_DEBOUNCE_MS);
}

// Incremental updates seem overkill in this scenario; so we only send a
// fresh new list every time here.

chrome.tabs.onCreated.addListener(() => sendCurrentTabsDebounced());
chrome.tabs.onUpdated.addListener(() => sendCurrentTabsDebounced());
chrome.tabs.onRemoved.addListener(() => sendCurrentTabsDebounced());
chrome.tabs.onActivated.addListener(() => sendCurrentTabsDebounced());

chrome.tabs.onMoved.addListener((tabId, moveInfo) => {
  // TODO: maybe handle this one day
});

function getGrantedPermissions() {
  return new Promise((resolve) => {
    chrome.permissions.getAll(({ permissions = [] }) => {
      resolve(permissions.filter((p) => OPTIONAL_PERMISSIONS.includes(p)));
    });
  });
}

function sendPermissions() {
  getGrantedPermissions().then((permissions) =>
    notifyNative("browser/permissions-changed", permissions),
  );
}

chrome.permissions.onAdded.addListener(() => sendPermissions());
chrome.permissions.onRemoved.addListener(() => sendPermissions());

function flattenBookmarks(nodes, folder, out) {
  for (const node of nodes) {
    if (node.url) {
      if (
        IGNORED_BOOKMARK_SCHEMES.some((scheme) => node.url.startsWith(scheme))
      )
        continue;
      out.push({
        id: node.id,
        title: node.title ?? "",
        url: node.url,
        folder,
        dateAdded: node.dateAdded ?? 0,
      });
    } else if (node.children) {
      flattenBookmarks(
        node.children,
        node.title ? [...folder, node.title] : folder,
        out,
      );
    }
  }
  return out;
}

// Wrap a callback-style extension API call into a promise that rejects on chrome.runtime.lastError.
function chromeCall(fn) {
  return new Promise((resolve, reject) => {
    fn((result) => {
      const error = chrome.runtime.lastError;
      if (error) reject(new Error(error.message));
      else resolve(result);
    });
  });
}

async function hasPermission(permission) {
  return chromeCall((cb) =>
    chrome.permissions.contains({ permissions: [permission] }, cb),
  );
}

async function requirePermission(permission, label) {
  if (!(await hasPermission(permission)))
    throw new Error(`${label} permission not granted`);
}

async function focusTab(tabId) {
  const tab = await chromeCall((cb) =>
    chrome.tabs.update(tabId, { active: true }, cb),
  );
  await chromeCall((cb) =>
    chrome.windows.update(tab.windowId, { focused: true }, cb),
  );
}

async function closeTab(tabId) {
  await chromeCall((cb) => chrome.tabs.remove(tabId, cb));
}

async function bookmarkTab(title, url) {
  if (!(await hasPermission("bookmarks"))) {
    await openPermissionPage("bookmarks");
    throw new Error("Bookmarks permission not granted");
  }
  await chromeCall((cb) => chrome.bookmarks.create({ title, url }, cb));
}

async function getBookmarks() {
  await requirePermission("bookmarks", "Bookmarks");

  const tree = await chromeCall((cb) => chrome.bookmarks.getTree(cb));
  // top-level nodes are the browser's built-in containers (bookmarks bar, other bookmarks...), not user folders
  const containers = tree.flatMap((root) => root.children ?? []);

  return containers.reduce(
    (out, container) => flattenBookmarks(container.children ?? [], [], out),
    [],
  );
}

async function removeBookmark(bookmarkId) {
  await requirePermission("bookmarks", "Bookmarks");
  await chromeCall((cb) => chrome.bookmarks.remove(bookmarkId, cb));
}

async function searchHistory(query, maxResults) {
  await requirePermission("history", "History");

  const results = await chromeCall((cb) =>
    chrome.history.search({ text: query ?? "", startTime: 0, maxResults }, cb),
  );

  return results
    .filter((item) => item.url)
    .map((item) => ({
      id: item.id,
      title: item.title ?? "",
      url: item.url,
      lastVisitTime: item.lastVisitTime ?? 0,
      visitCount: item.visitCount ?? 0,
    }));
}

async function removeHistoryUrl(url) {
  if (typeof url !== "string" || !url) throw new Error("Invalid URL");
  await requirePermission("history", "History");
  await chromeCall((cb) => chrome.history.deleteUrl({ url }, cb));
}

async function clearHistory() {
  await requirePermission("history", "History");
  await chromeCall((cb) => chrome.history.deleteAll(cb));
}

// Permission prompts need a user gesture, which vicinae cannot provide: open a page with a button instead.
async function openPermissionPage(permission) {
  if (!OPTIONAL_PERMISSIONS.includes(permission))
    throw new Error(`Unknown permission ${permission}`);

  const url = chrome.runtime.getURL(
    `permissions.html?permission=${encodeURIComponent(permission)}`,
  );
  const tab = await chromeCall((cb) => chrome.tabs.create({ url }, cb));

  await chromeCall((cb) =>
    chrome.windows.update(tab.windowId, { focused: true }, cb),
  );
}

async function start() {
  const storage =
    typeof browser === "undefined"
      ? chrome.storage.local
      : browser.storage.local;
  const saved = await storage.get(["browserId", "profileName"]);
  browserId = saved.browserId || crypto.randomUUID();
  profileName = typeof saved.profileName === "string" ? saved.profileName : "";
  if (!saved.browserId) await storage.set({ browserId });
  ({ name: browserName, engine: browserEngine } = await detectBrowser());
  if (!manuallyDisconnected) connectToNativeHost();
}

console.log("background service loaded");
setConnectionState("disconnected");
start().catch((error) => {
  console.error("Could not initialize browser integration:", error);
  setConnectionState("error", error.message);
});
