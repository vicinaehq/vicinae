const HOST_NAME = "com.vicinae.vicinae";
const RETRY_INITIAL_MS = 1000;
const RETRY_MAX_MS = 60000;
const TAB_DEBOUNCE_MS = 50;
const OPTIONAL_PERMISSIONS = ["bookmarks"];
const IGNORED_BOOKMARK_SCHEMES = ["place:", "javascript:"];
const browserId = crypto.randomUUID();
const requestMap = new Map();

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
	chrome.runtime.sendMessage({
		source: "connection_state",
		state,
		manuallyDisconnected,
		...(error && { error })
	}).catch(() => { });
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
		return ;
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

// we handle no request, only notifications for now
function handleRequest(type, data) {
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

function handleNotification(type, data) {
	if (type === "browser/focus-tab") {
		const { tabId } = data;

		chrome.tabs.get(tabId, (tab) => {
			chrome.tabs.update(tabId, { active: true });
			chrome.windows.update(tab.windowId, { focused: true });
		});
	}

	if (type === "browser/close-tab") {
		const { tabId } = data;
		chrome.tabs.remove(tabId);
	}

	if (type === "browser/request-bookmarks") {
		sendBookmarks(data.requestId);
	}

	if (type === "browser/request-permission") {
		openPermissionPage(data.permission);
	}

	if (type === "browser/remove-bookmark") {
		chrome.bookmarks?.remove(data.bookmarkId);
	}
}

function processNativeMessage(message) {
	console.log('got message:', message);

	if (message.method) {
		if (message.id) {
			return handleRequest(message.id, message.data);
		}
		return handleNotification(message.method, message.data);
	}

	return handleResponse(message.id, message.result, message.error);
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

		Promise.all([getBrowserName(), getGrantedPermissions()]).then(([name, permissions]) => {
			requestToNative("browser/init", {
				id: browserId,
				name,
				engine: 'chromium',
				version: chrome.runtime.getManifest().version,
				permissions,
			}, (data, error) => {
				if (!error) {
					retryDelay = RETRY_INITIAL_MS;
					setConnectionState("connected");
					sendCurrentTabs();
				}
			});
		});

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
		if (connectionState !== "connected") {
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
	} else if (message.type === "get_extension_id") {
		sendResponse({ extensionId: chrome.runtime.id });
	}
	return true;
});


function sendCurrentTabs() {
	chrome.tabs.query({}, (tabs) => {
		notifyNative('browser/tabs-changed', tabs.map(tab => ({
			id: tab.id,
			title: tab.title,
			url: tab.url,
			windowId: tab.windowId,
			active: tab.active,
			audible: tab.audible ?? false,
			muted: tab.mutedInfo?.muted ?? false,
			lastAccessed: tab.lastAccessed ?? 0,
			pinned: tab.pinned ?? false
		})));
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

function getBrowserName() {
	if (globalThis.browser?.runtime?.getBrowserInfo) {
		return browser.runtime.getBrowserInfo().then((info) => info.name);
	}

	const brands = navigator.userAgentData?.brands ?? [];
	const brand = brands.find((b) => !/Not.?A.?Brand|Chromium/i.test(b.brand)) ?? brands.find((b) => b.brand === "Chromium");

	return Promise.resolve(brand?.brand ?? navigator.userAgent);
}

function getGrantedPermissions() {
	return new Promise((resolve) => {
		chrome.permissions.getAll(({ permissions = [] }) => {
			resolve(permissions.filter((p) => OPTIONAL_PERMISSIONS.includes(p)));
		});
	});
}

function sendPermissions() {
	getGrantedPermissions().then((permissions) => notifyNative("browser/permissions-changed", permissions));
}

chrome.permissions.onAdded.addListener(() => sendPermissions());
chrome.permissions.onRemoved.addListener(() => sendPermissions());

function flattenBookmarks(nodes, folder, out) {
	for (const node of nodes) {
		if (node.url) {
			if (IGNORED_BOOKMARK_SCHEMES.some((scheme) => node.url.startsWith(scheme))) continue;
			out.push({
				id: node.id,
				title: node.title ?? "",
				url: node.url,
				folder,
				dateAdded: node.dateAdded ?? 0,
			});
		} else if (node.children) {
			flattenBookmarks(node.children, node.title ? [...folder, node.title] : folder, out);
		}
	}
	return out;
}

function sendBookmarks(requestId) {
	if (!chrome.bookmarks) {
		notifyNative("browser/bookmarks", { requestId, bookmarks: [], error: "Bookmarks permission not granted" });
		return;
	}

	chrome.bookmarks.getTree((tree) => {
		if (chrome.runtime.lastError) {
			notifyNative("browser/bookmarks", { requestId, bookmarks: [], error: chrome.runtime.lastError.message });
			return;
		}
		// top-level nodes are the browser's built-in containers (bookmarks bar, other bookmarks...), not user folders
		const containers = tree.flatMap((root) => root.children ?? []);
		const bookmarks = containers.reduce((out, container) => flattenBookmarks(container.children ?? [], [], out), []);

		notifyNative("browser/bookmarks", { requestId, bookmarks });
	});
}

// Permission prompts need a user gesture, which vicinae cannot provide: open a page with a button instead.
function openPermissionPage(permission) {
	if (!OPTIONAL_PERMISSIONS.includes(permission)) return;

	const url = chrome.runtime.getURL(`permissions.html?permission=${encodeURIComponent(permission)}`);

	chrome.tabs.create({ url }, (tab) => {
		chrome.windows.update(tab.windowId, { focused: true });
	});
}

console.log("background service loaded");
setConnectionState("disconnected");
connectToNativeHost();
