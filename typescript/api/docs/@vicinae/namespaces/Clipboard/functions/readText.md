[**@vicinae/api**](../../../../README.md)

***

[@vicinae/api](../../../../README.md) / [Clipboard](../README.md) / readText

# Function: readText()

> **readText**(`options?`): `Promise`\<`string`\>

Read the text representation of the current clipboard data. If the data is not text at all, this 
returns an empty string.
If you want to read optional html or file path, consider

## Parameters

### options?

#### offset?

`number`

## Returns

`Promise`\<`string`\>

## See

Note: the offset option is not yet implemented.

```ts
const text = await Clipboard.readText();
```
