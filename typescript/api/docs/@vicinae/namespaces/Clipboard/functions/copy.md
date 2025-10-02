[**@vicinae/api**](../../../../README.md)

***

[@vicinae/api](../../../../README.md) / [Clipboard](../README.md) / copy

# Function: copy()

> **copy**(`text`, `options`): `Promise`\<`void`\>

Copy the provided content in the clipboard.
The `concealed` option can be passed so that the created clipboard selection
does not get indexed by the Vicinae clipboard manager.

## Parameters

### text

`string` | `number` | [`Content`](../type-aliases/Content.md)

### options

[`CopyOptions`](../type-aliases/CopyOptions.md) = `{}`

## Returns

`Promise`\<`void`\>
