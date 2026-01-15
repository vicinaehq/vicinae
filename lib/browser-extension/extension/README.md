# Chromium IPC Extension

A browser extension that spawns a native process and communicates via IPC using Chromium's Native Messaging API.

## Architecture

```
┌─────────────────────────────────────┐
│  Browser Extension (JavaScript)      │
│  - background.js (Service Worker)   │
│  - popup.html/js (UI)               │
└──────────────┬──────────────────────┘
               │ Native Messaging API
               │ (JSON over stdin/stdout)
               │
┌──────────────┴──────────────────────┐
│  Native Host Process (Python)       │
│  - ipc_host.py                      │
│  - Runs as separate process         │
└─────────────────────────────────────┘
```

## Message Protocol

Messages are exchanged using Chromium's Native Messaging protocol:
- **Format**: 4-byte length prefix (uint32) + JSON message
- **Transport**: stdin/stdout
- **Encoding**: UTF-8 JSON

### Example Messages

**Extension → Native Host:**
```json
{
  "type": "message",
  "content": "Hello from extension",
  "timestamp": 1704556800000
}
```

**Native Host → Extension:**
```json
{
  "type": "response",
  "original": "Hello from extension",
  "processed": "HELLO FROM EXTENSION",
  "timestamp": "2026-01-06T18:34:00"
}
```

## Installation

### 1. Install the Extension

1. Open Chromium/Chrome and navigate to `chrome://extensions/`
2. Enable "Developer mode" (toggle in top-right)
3. Click "Load unpacked"
4. Select the `chromium` directory containing `manifest.json`
5. **Copy the Extension ID** (you'll need this for step 2)

### 2. Install the Native Messaging Host

The native host manifest must be installed in a specific location:

**For Chromium (Linux):**
```bash
mkdir -p ~/.config/chromium/NativeMessagingHosts/
cp native-host/com.vicinaeq.ipc_host.json ~/.config/chromium/NativeMessagingHosts/
```

**For Google Chrome (Linux):**
```bash
mkdir -p ~/.config/google-chrome/NativeMessagingHosts/
cp native-host/com.vicinaeq.ipc_host.json ~/.config/google-chrome/NativeMessagingHosts/
```

**For system-wide installation:**
```bash
sudo mkdir -p /etc/chromium/native-messaging-hosts/
sudo cp native-host/com.vicinaeq.ipc_host.json /etc/chromium/native-messaging-hosts/
```

### 3. Update the Host Manifest

Edit the installed `com.vicinaeq.ipc_host.json` and replace `YOUR_EXTENSION_ID_HERE` with your actual extension ID:

```json
{
  "name": "com.vicinaeq.ipc_host",
  "description": "IPC Native Messaging Host",
  "path": "/home/aurelle/prog/perso/vicinaehq/browser-extension/chromium/native-host/ipc_host.py",
  "type": "stdio",
  "allowed_origins": [
    "chrome-extension://abcdefghijklmnopqrstuvwxyz123456/"
  ]
}
```

### 4. Test the Connection

1. Click the extension icon in the browser toolbar
2. Click "Connect to Native Host"
3. Type a message and click "Send Message"
4. You should see responses from the native host

## Development

### Debugging

**Extension side:**
- Right-click extension icon → "Inspect popup" for popup console
- Go to `chrome://extensions/` → Click "service worker" link for background script console

**Native host side:**
- Check logs at `/tmp/ipc_host.log`
```bash
tail -f /tmp/ipc_host.log
```

### Modifying the Native Host

Edit `native-host/ipc_host.py` to customize message handling:

```python
def handle_message(message):
    msg_type = message.get('type', 'unknown')

    if msg_type == 'your_custom_type':
        # Your custom logic here
        response = {
            'type': 'custom_response',
            'data': 'your data'
        }
        send_message(response)
```

### Talking to the Native Host from Another Process

The native host communicates via stdin/stdout, so you can spawn it from any process and use the same message protocol. Here's a Python example:

```python
import subprocess
import struct
import json

# Start the native host
proc = subprocess.Popen(
    ['./native-host/ipc_host.py'],
    stdin=subprocess.PIPE,
    stdout=subprocess.PIPE,
    bufsize=0
)

def send_to_host(message):
    encoded = json.dumps(message).encode('utf-8')
    length = struct.pack('I', len(encoded))
    proc.stdin.write(length + encoded)
    proc.stdin.flush()

def read_from_host():
    raw_length = proc.stdout.read(4)
    if not raw_length:
        return None
    length = struct.unpack('I', raw_length)[0]
    data = proc.stdout.read(length)
    return json.loads(data.decode('utf-8'))

# Send a message
send_to_host({'type': 'message', 'content': 'Hello'})

# Read response
response = read_from_host()
print(response)
```

## File Structure

```
chromium/
├── manifest.json           # Extension manifest
├── background.js           # Service worker (IPC handler)
├── popup.html             # Extension popup UI
├── popup.js               # Popup logic
├── README.md              # This file
└── native-host/
    ├── ipc_host.py        # Native messaging host (Python)
    └── com.vicinaeq.ipc_host.json  # Host manifest
```

## Security Considerations

- Only extensions listed in `allowed_origins` can connect to the native host
- The native host runs with the same permissions as the user
- Validate all messages in both directions
- Never execute arbitrary code from messages without validation

## Troubleshooting

**"Specified native messaging host not found"**
- Verify the host manifest is in the correct directory
- Check that the extension ID in `allowed_origins` matches your extension
- Ensure the `path` in the manifest points to the correct location

**Native host not responding**
- Check if `ipc_host.py` is executable: `chmod +x native-host/ipc_host.py`
- Verify Python 3 is installed: `python3 --version`
- Check the log file: `cat /tmp/ipc_host.log`

**Permission errors**
- Ensure the native host script has execute permissions
- Check file paths are absolute in the host manifest
