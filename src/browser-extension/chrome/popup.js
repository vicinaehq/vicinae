// Popup script for Vicinae Extension

const statusPin = document.getElementById('statusPin');
const statusText = document.getElementById('statusText');
const toggleBtn = document.getElementById('toggleBtn');

let currentState = 'disconnected';

function setStatus(state, text) {
  statusPin.className = 'status-pin ' + state;
  statusText.className = 'status-text ' + state;
  statusText.textContent = text;
}

// Check connection state on popup open
chrome.runtime.sendMessage({ type: 'get_connection_state' }, (response) => {
  if (response) {
    updateFromState(response.state, response.manuallyDisconnected);
  }
});

function updateFromState(state, manuallyDisconnected) {
  currentState = state;

  if (state === 'connected') {
    setStatus('connected', 'Connected');
    toggleBtn.textContent = 'Disconnect';
    toggleBtn.className = 'disconnect';
  } else if (state === 'error') {
    setStatus('error', 'Connection failed');
    toggleBtn.textContent = 'Reconnect';
    toggleBtn.className = 'connect';
  } else if (state === 'connecting' && !manuallyDisconnected) {
    setStatus('disconnected', 'Connecting...');
    toggleBtn.textContent = 'Disconnect';
    toggleBtn.className = 'disconnect';
  } else {
    setStatus('disconnected', 'Not connected');
    toggleBtn.textContent = 'Connect';
    toggleBtn.className = 'connect';
  }
}

toggleBtn.addEventListener('click', () => {
  if (currentState === 'connected' || currentState === 'connecting') {
    chrome.runtime.sendMessage({ type: 'disconnect_connection' });
  } else {
    chrome.runtime.sendMessage({ type: 'retry_connection' });
  }
});

// Listen for connection state changes
chrome.runtime.onMessage.addListener((message) => {
  if (message.source === 'connection_state') {
    updateFromState(message.state, message.manuallyDisconnected);
  }
});
