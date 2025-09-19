[**@vicinae/api**](../../../README.md)

***

[@vicinae/api](../../../README.md) / FileSearch

# FileSearch

Access Vicinae's built-in file search functionality.

## Remarks

This API provides read-only access to the file search system.
Use this to find existing files indexed by Vicinae.
This does **not** provide indexing capabilities.

## Example

```typescript
import { FileSearch } from '@vicinae/api';

const results = await FileSearch.search('invoice');
console.log(`Found ${results.length} files`);
```

## Type Aliases

- [FileInfo](type-aliases/FileInfo.md)
- [SearchOptions](type-aliases/SearchOptions.md)

## Functions

- [search](functions/search.md)
