// Background service worker for IPC Extension
// This handles communication with the native host process

const HOST_NAME = "com.vicinae.vicinae";
const RETRY_EVERY_MS = 10000;
let port = null;
let connectionState = "disconnected"; // disconnected, connecting, connected, error

let retryId;
const browserId = crypto.randomUUID();

// Connect to native host on startup
function connectToNativeHost() {
	console.log("Connecting to native host:", HOST_NAME);
	connectionState = "connecting";

	clearTimeout(retryId);

	try {
		port = chrome.runtime.connectNative(HOST_NAME);

		// Handle messages from native host
		port.onMessage.addListener((message) => {
			console.log("Received from native host:", message);

			if (message.method == "init") {
				replyToNative(message.id, {
					browserId
				});
			}

			if (message.method == "browser/focus-tab") {
				const { tabId } = message.data;

				chrome.tabs.get(tabId, (tab) => {
					chrome.tabs.update(tabId, { active: true });
					chrome.windows.update(tab.windowId, { focused: true });
				});

				return;
			}


			// Broadcast to all extension contexts (popup, content scripts, etc.)
			chrome.runtime.sendMessage({
				source: "native_host",
				data: message
			}).catch((error) => {
				// Ignore errors if no listeners
			});

			// Broadcast connection state change
			chrome.runtime.sendMessage({
				source: "connection_state",
				state: "connected"
			}).catch(() => { });
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

function replyToNative(id, data) {
	sendToNativeHost({
		id,
		result: data
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

console.log("IPC Extension background service worker loaded2");
