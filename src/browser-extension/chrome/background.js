const HOST_NAME = "com.vicinae.vicinae";
const RETRY_INITIAL_MS = 1000;
const RETRY_MAX_MS = 60000;
const TAB_DEBOUNCE_MS = 50;
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

		requestToNative("browser/init", {
			id: browserId,
			name: navigator.userAgent,
			engine: 'chromium'
		}, (data, error) => {
			if (!error) {
				retryDelay = RETRY_INITIAL_MS;
				setConnectionState("connected");
				sendCurrentTabs();
			}
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
			audible: tab.audible,
			muted: tab.mutedInfo?.muted ?? false
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

console.log("background service loaded");
setConnectionState("disconnected");
connectToNativeHost();
