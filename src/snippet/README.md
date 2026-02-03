# vicinae snippet server

This library exports a snippet server that can register snippets and send notifications when expansion is required.

The main vicinae server will start the snippet server by calling itself with a specific set of arguments.

Communication between the main vicinae server and the snippet server is done using stdin and stdout. stderr is used for debug logs.

The vicinae server typically starts the snipet server, registers snippets that were persisted, and then starts listening for expansions.

The vicinae server receives an expansion notification when a given snippet is triggered, and shall populate the clipboard with whatever that specific snippet needs to be expanded to.

It then sends another request to the snippet server in order to inject ctrl+v or ctrl+shift+v at the cursor position.

Expansion is performed in vicinae because vicinae already has all the context required for expansion, and complex expansion types may require accessing vicinae data directly (focused window, active app, clipboard contents...).

For the snippet server to work as intended, relevant udev rules need to be enabled. In most cases, this is done through the vicinae installation process.
