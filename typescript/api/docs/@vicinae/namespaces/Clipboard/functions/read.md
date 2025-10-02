[**@vicinae/api**](../../../../README.md)

***

[@vicinae/api](../../../../README.md) / [Clipboard](../README.md) / read

# Function: read()

> **read**(`options?`): `Promise`\<[`ReadContent`](../type-aliases/ReadContent.md)\>

Read the current content of the clipboard, which can contain text, html and a file path.
Note: the offset option is not yet implemented

```ts
const { text, html, file } = await Clipboard.read();
```

## Parameters

### options?

#### offset?

`number`

## Returns

`Promise`\<[`ReadContent`](../type-aliases/ReadContent.md)\>
