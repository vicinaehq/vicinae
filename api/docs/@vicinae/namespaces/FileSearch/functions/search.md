[**@vicinae/api**](../../../../README.md)

***

[@vicinae/api](../../../../README.md) / [FileSearch](../README.md) / search

# Function: search()

> **search**(`query`, `_`): `Promise`\<`FileInfo`[]\>

Defined in: [file-search.ts:49](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/file-search.ts#L49)

Search for files matching the provided query string.

## Parameters

### query

`string`

Search term (min. 1 character) - the shorter the query the longer the average search takes.

### \_

[`SearchOptions`](../type-aliases/SearchOptions.md) = `{}`

## Returns

`Promise`\<`FileInfo`[]\>

Promise resolving to array of matching files

## Remarks

Uses prefix matching on filename tokens. For example:
- File: "invoice-new-motherboard.pdf" 
- Matches: "inv", "new", "mother", "pdf"
- No match: "board", "oice" (not prefixes)

## Example

```typescript
const files = await fileSearch.search('invoice');
```
