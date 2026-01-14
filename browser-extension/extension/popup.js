// Popup script for Vicinae IPC Extension

const messagesDiv = document.getElementById('messages');
const messageInput = document.getElementById('messageInput');
const statusBar = document.getElementById('statusBar');
const statusText = document.getElementById('statusText');
const setupInstructions = document.getElementById('setupInstructions');
const mainUI = document.getElementById('mainUI');
const setupCommand = document.getElementById('setupCommand');

let extensionId = '';

// Update UI based on connection state
function updateConnectionState(state, error = null) {
  statusBar.className = 'status ' + state;

  if (state === 'connected') {
    statusText.textContent = 'Connected to Vicinae';
    setupInstructions.classList.add('hidden');
    mainUI.classList.remove('hidden');
  } else if (state === 'error') {
    statusText.textContent = 'Setup Required';
    setupInstructions.classList.remove('hidden');
    mainUI.classList.add('hidden');
  } else {
    statusText.textContent = 'Disconnected';
    setupInstructions.classList.remove('hidden');
    mainUI.classList.add('hidden');
  }
}

function setConnected(version, pid) {
    statusBar.className = 'status ' + 'connected';
	statusText.textContent = `Connected to Vicinae ${version} (pid=${pid})`;
    setupInstructions.classList.add('hidden');
    mainUI.classList.remove('hidden');
}

// Initialize popup
async function initialize() {
  // Get extension ID
  chrome.runtime.sendMessage({ type: 'get_extension_id' }, (response) => {
    if (response && response.extensionId) {
      extensionId = response.extensionId;
    }
  });


chrome.runtime.sendMessage({
    target: 'native_host',
    data: {
	  id: 0,
      method: 'version',
	  data: {}
    }
  }, (response) => {
	  console.log({ response });
    if (response && response.success) {
      addMessage(`Sent: ${message}`, 'sent');
      messageInput.value = '';
    }
  });


	/*
  // Check connection state
  chrome.runtime.sendMessage({ type: 'get_connection_state' }, (response) => {
    if (response) {
      updateConnectionState(response.state);
    }
  });
  */
}

function addMessage(text, type = 'info') {
  const messageEl = document.createElement('div');
  messageEl.className = 'message';
  messageEl.textContent = `[${type}] ${text}`;
  messagesDiv.appendChild(messageEl);
  messagesDiv.scrollTop = messagesDiv.scrollHeight;
}

// Send button
document.getElementById('send').addEventListener('click', () => {
  const message = messageInput.value.trim();
  if (!message) return;

  chrome.runtime.sendMessage({
    target: 'native_host',
    data: {
      type: 'message',
      content: message,
      timestamp: Date.now()
    }
  }, (response) => {
    if (response && response.success) {
      addMessage(`Sent: ${message}`, 'sent');
      messageInput.value = '';
    }
  });
});

// Listen for messages from native host (via background)
chrome.runtime.onMessage.addListener((message, sender, sendResponse) => {
	console.log({ message });
  if (message.source === 'native_host') {
	  if (message.data.type == "version") {
		  const { tag, pid } = message.data.data;
		setConnected(tag, pid);
	  }
    addMessage(JSON.stringify(message.data), 'received');
  } else if (message.source === 'connection_state') {
    //updateConnectionState(message.state, message.error);
  }
});

// Enter key support
messageInput.addEventListener('keypress', (e) => {
  if (e.key === 'Enter') {
    document.getElementById('send').click();
  }
});

// Initialize on load
initialize();
