[**@vicinae/api**](../../../../README.md)

***

[@vicinae/api](../../../../README.md) / [Clipboard](../README.md) / paste

# Function: paste()

> **paste**(`text`): `Promise`\<`void`\>

Paste the provided clipboard content to the active window.
If the environment does not support either getting the active window
or pasting content to it directly, this will fallback to a regular
clipboard copy.

## Parameters

### text

`string` | [`Content`](../type-aliases/Content.md)

## Returns

`Promise`\<`void`\>
