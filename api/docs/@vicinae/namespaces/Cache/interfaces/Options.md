[**@vicinae/api**](../../../../README.md)

***

[@vicinae/api](../../../../README.md) / [Cache](../README.md) / Options

# Interface: Options

The options for creating a new [Cache](../../../../classes/Cache.md).

## Properties

### capacity?

> `optional` **capacity**: `number`

The capacity in bytes. If the stored data exceeds the capacity, the least recently used data is removed.
The default capacity is 10 MB.

***

### namespace?

> `optional` **namespace**: `string`

If set, the Cache will be namespaced via a subdirectory.
This can be useful to separate the caches for individual commands of an extension.
By default, the cache is shared between the commands of an extension.
