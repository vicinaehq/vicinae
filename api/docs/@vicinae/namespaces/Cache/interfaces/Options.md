[**@vicinae/api**](../../../../README.md)

***

[@vicinae/api](../../../../README.md) / [Cache](../README.md) / Options

# Interface: Options

Defined in: [cache.ts:10](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/cache.ts#L10)

The options for creating a new [Cache](../../../../classes/Cache.md).

## Properties

### capacity?

> `optional` **capacity**: `number`

Defined in: [cache.ts:21](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/cache.ts#L21)

The capacity in bytes. If the stored data exceeds the capacity, the least recently used data is removed.
The default capacity is 10 MB.

***

### namespace?

> `optional` **namespace**: `string`

Defined in: [cache.ts:16](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/cache.ts#L16)

If set, the Cache will be namespaced via a subdirectory.
This can be useful to separate the caches for individual commands of an extension.
By default, the cache is shared between the commands of an extension.
