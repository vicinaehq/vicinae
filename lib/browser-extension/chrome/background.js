// Background service worker for IPC Extension
// This handles communication with the native host process

const HOST_NAME = "com.vicinae.vicinae";
const RETRY_EVERY_MS = 10000;
let port = null;
let connectionState = "disconnected"; // disconnected, connecting, connected, error

let retryId;
const browserId = crypto.randomUUID();

const requestMap = new Map();
let serial = 0;

function requestToNative(type, data, cb) {
	const id = serial++;

	requestMap.set(id, cb);
	sendToNativeHost({
		id,
		method: type,
		data,
	});
}

// Send message to native host
function sendToNativeHost(message) {
	if (!port) {
		console.log("Not connected, attempting to connect...");
		port = connectToNativeHost();
	}

	if (port) {
		console.log("Sending to native host:", message);
		port.postMessage(message);
	} else {
		console.error("Failed to connect to native host");
	}
}

function handleRequest(type, data) {
}

function handleResponse(id, result, error) {
	const handler = requestMap.get(id);

	if (!handler) {
		console.error(`No handler for request with id ${id}`);
		return;
	}

	handler(result, error);
}

function handleNotification(type, data) {
	if (type == "browser/focus-tab") {
		const { tabId } = data;

		chrome.tabs.get(tabId, (tab) => {
			chrome.tabs.update(tabId, { active: true });
			chrome.windows.update(tab.windowId, { focused: true });
		});
	}
}

function processNativeMessage(message) {
	console.log('got message', message);
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
	connectionState = "connecting";

	clearTimeout(retryId);

	try {
		port = chrome.runtime.connectNative(HOST_NAME);
		requestMap.clear();

		requestToNative("browser/init", {
			id: browserId,
			name: "Random browser"
		}, (data, error) => {
			if (!error) {
				sendCurrentTabs();
			}
		});

		// Handle messages from native host
		port.onMessage.addListener((message) => {
			console.log("Received from native host:", message);
			processNativeMessage(message);
		});

		// Handle disconnect
		port.onDisconnect.addListener(() => {
			console.log("Disconnected from native host");
			const error = chrome.runtime.lastError;

			retryId = setTimeout(() => {
				connectToNativeHost();
			}, RETRY_EVERY_MS);

			if (error) {
				console.error("Disconnect error:", error.message);
				connectionState = "error";

				// Broadcast error state with details
				chrome.runtime.sendMessage({
					source: "connection_state",
					state: "error",
					error: error.message
				}).catch(() => { });
			} else {
				connectionState = "disconnected";
				chrome.runtime.sendMessage({
					source: "connection_state",
					state: "disconnected"
				}).catch(() => { });
			}

			port = null;
		});

		return port;
	} catch (error) {
		console.error("Failed to connect:", error);
		connectionState = "error";
		chrome.runtime.sendMessage({
			source: "connection_state",
			state: "error",
			error: error.message
		}).catch(() => { });
		return null;
	}
}



// Listen for messages from extension contexts (popup, content scripts)
chrome.runtime.onMessage.addListener((message, sender, sendResponse) => {
	if (message.target === "native_host") {
		sendToNativeHost(message.data);
		sendResponse({ success: true });
	} else if (message.type === "get_connection_state") {
		sendResponse({ state: connectionState });
	} else if (message.type === "get_extension_id") {
		sendResponse({ extensionId: chrome.runtime.id });
	}
	return true;
});

function sendToNative(type, data) {
	sendToNativeHost({
		id: 0,
		method: type,
		data,
	});
}

function sendCurrentTabs() {
	chrome.tabs.query({}, (tabs) => {
		sendToNative('browser/tabs-changed', tabs.map(tab => ({
			id: tab.id,
			title: tab.title,
			url: tab.url,
			windowId: tab.windowId,
			active: tab.active
		})));
	});
}

// Incremental updates seem overkill in this scenario; so we only send a 
// fresh new list every time here.

chrome.tabs.onCreated.addListener((tab) => {
	sendCurrentTabs();
});

chrome.tabs.onUpdated.addListener((tabId, changeInfo, tab) => {
	sendCurrentTabs();
});

chrome.tabs.onRemoved.addListener((tabId, removeInfo) => {
	sendCurrentTabs();
});

chrome.tabs.onActivated.addListener((activeInfo) => {
	sendCurrentTabs(); // maybe remove that if that's too much
});

chrome.tabs.onMoved.addListener((tabId, moveInfo) => {
	// TODO: maybe handle this one day
});


// Auto-connect on startup
connectToNativeHost();

console.log("IPC Extension background service worker loaded");
