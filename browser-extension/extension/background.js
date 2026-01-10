// Background service worker for IPC Extension
// This handles communication with the native host process

const HOST_NAME = "com.vicinaehq.vicinae";
let port = null;
let connectionState = "disconnected"; // disconnected, connecting, connected, error

// Connect to native host on startup
function connectToNativeHost() {
  console.log("Connecting to native host:", HOST_NAME);
  connectionState = "connecting";

  try {
    port = chrome.runtime.connectNative(HOST_NAME);

    // Handle messages from native host
    port.onMessage.addListener((message) => {
      console.log("Received from native host:", message);
      connectionState = "connected";

      // Broadcast to all extension contexts (popup, content scripts, etc.)
      chrome.runtime.sendMessage({
        source: "native_host",
        data: message
      }).catch(() => {
        // Ignore errors if no listeners
      });

      // Broadcast connection state change
      chrome.runtime.sendMessage({
        source: "connection_state",
        state: "connected"
      }).catch(() => {});
    });

    // Handle disconnect
    port.onDisconnect.addListener(() => {
      console.log("Disconnected from native host");
      const error = chrome.runtime.lastError;

      if (error) {
        console.error("Disconnect error:", error.message);
        connectionState = "error";

        // Broadcast error state with details
        chrome.runtime.sendMessage({
          source: "connection_state",
          state: "error",
          error: error.message
        }).catch(() => {});
      } else {
        connectionState = "disconnected";
        chrome.runtime.sendMessage({
          source: "connection_state",
          state: "disconnected"
        }).catch(() => {});
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
    }).catch(() => {});
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


// Auto-connect on startup
connectToNativeHost();
sendToNativeHost({"hello": "hilou"});

console.log("IPC Extension background service worker loaded2");
