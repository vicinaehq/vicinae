// Popup script for Vicinae Extension

const statusPin = document.getElementById('statusPin');
const statusText = document.getElementById('statusText');
const toggleBtn = document.getElementById('toggleBtn');
const profileForm = document.getElementById('profileForm');
const profileName = document.getElementById('profileName');
const profileStatus = document.getElementById('profileStatus');

const extensionApi = typeof browser === 'undefined' ? chrome : browser;

extensionApi.storage.local.get('profileName').then(({ profileName: saved }) => {
  profileName.value = saved ?? '';
}).catch(() => {
  profileStatus.textContent = 'Could not load profile label';
});

profileForm.addEventListener('submit', async (event) => {
  event.preventDefault();
  const name = profileName.value.trim();
  try {
    await extensionApi.storage.local.set({ profileName: name });
    profileName.value = name;
  } catch (error) {
    profileStatus.textContent = 'Could not save profile label';
    console.error(error);
    return;
  }

  try {
    const response = await extensionApi.runtime.sendMessage({ type: 'profile_name_changed', name });
    profileStatus.textContent = response?.success ? 'Saved' : 'Saved; reconnect to update Vicinae';
  } catch (error) {
    profileStatus.textContent = 'Saved; reconnect to update Vicinae';
    console.error(error);
  }
});

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

const bookmarksToggle = document.getElementById('bookmarksToggle');
const BOOKMARKS = { permissions: ['bookmarks'] };

function refreshBookmarksToggle() {
  chrome.permissions.contains(BOOKMARKS, (granted) => {
    bookmarksToggle.checked = granted;
  });
}

bookmarksToggle.addEventListener('change', () => {
  if (bookmarksToggle.checked) {
    chrome.permissions.request(BOOKMARKS, () => refreshBookmarksToggle());
  } else {
    chrome.permissions.remove(BOOKMARKS, () => refreshBookmarksToggle());
  }
});

refreshBookmarksToggle();
