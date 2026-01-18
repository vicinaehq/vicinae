// Popup script for Vicinae Extension

const statusPin = document.getElementById('statusPin');
const statusText = document.getElementById('statusText');

function setStatus(state, text) {
  statusPin.className = 'status-pin ' + state;
  statusText.className = 'status-text ' + state;
  statusText.textContent = text;
}

// Check connection state and trigger retry if not connected
chrome.runtime.sendMessage({ type: 'get_connection_state' }, (response) => {
  if (response) {
    updateFromState(response.state);
    if (response.state !== 'connected') {
      chrome.runtime.sendMessage({ type: 'retry_connection' });
    }
  }
});

function updateFromState(state) {
  if (state === 'connected') {
    setStatus('connected', 'Connected');
  } else if (state === 'error') {
    setStatus('error', 'Not connected');
  } else {
    setStatus('disconnected', 'Connecting...');
  }
}

// Listen for connection state changes
chrome.runtime.onMessage.addListener((message) => {
  if (message.source === 'connection_state') {
    updateFromState(message.state);
  }
});
