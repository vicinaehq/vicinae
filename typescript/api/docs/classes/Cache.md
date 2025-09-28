[**@vicinae/api**](../README.md)

***

[@vicinae/api](../README.md) / Cache

# Class: Cache

Caching abstraction that stores data on disk and supports LRU (least recently used) access.
Values can only be stored as plain text strings, so it is up to you to serialize your data in an appropriate way.
For instance, you could store json using `JSON.stringify` and `JSON.parse` it back.
If you need to store binary data, you could encode it in base64.

Unlike the local storage API, this API exclusively uses the extension's support directory to store its data.
No calls to internal Vicinae APIs are required, hence why all methods in this class are synchronous.
Another major difference is that cache data is not encrypted, so it's not suitable to store secrets. 
(local storage currently isn't either, but that will change in the future)

## Constructors

### Constructor

> **new Cache**(`options?`): `Cache`

#### Parameters

##### options?

[`Options`](../@vicinae/namespaces/Cache/interfaces/Options.md)

#### Returns

`Cache`

## Accessors

### isEmpty

#### Get Signature

> **get** **isEmpty**(): `boolean`

##### Returns

`boolean`

whether the cache is empty.

***

### storageDirectory

#### Get Signature

> **get** **storageDirectory**(): `string`

##### Remarks

This is provided for informative purpose only. You should not attempt to mutate anything
in this directory by yourself. Also note that the way cache data is serialized on disk can (will) change
across versions.

##### Returns

`string`

the full path to the directory where the data is stored on disk.

## Methods

### clear()

> **clear**(`options?`): `void`

Clears all stored data.
This also notifies registered subscribers (see [subscribe](#subscribe)) unless the  `notifySubscribers` option is set to `false`.

#### Parameters

##### options?

###### notifySubscribers

`boolean`

#### Returns

`void`

***

### get()

> **get**(`key`): `undefined` \| `string`

#### Parameters

##### key

`string`

#### Returns

`undefined` \| `string`

the data for the given key, or `undefined` if there is no data.

#### Remarks

To solely check for existence of a key, use [has](#has).

***

### has()

> **has**(`key`): `boolean`

#### Parameters

##### key

`string`

#### Returns

`boolean`

`true` if data for the key exists, `false` otherwise.

#### Remarks

You can use this method to check for entries without affecting the LRU access.

***

### remove()

> **remove**(`key`): `boolean`

Removes the data for the given key.
This also notifies registered subscribers (see [subscribe](#subscribe)).

#### Parameters

##### key

`string`

#### Returns

`boolean`

`true` if data for the key was removed, `false` otherwise.

***

### set()

> **set**(`key`, `data`): `void`

Sets the data for the given key.
If the data exceeds the configured `capacity`, the least recently used entries are removed.
This also notifies registered subscribers (see [subscribe](#subscribe)).

#### Parameters

##### key

`string`

##### data

`string`

#### Returns

`void`

#### Remarks

An individual cache entry cannot be bigger than the configured capacity. If this happens, an error will be thrown.

***

### subscribe()

> **subscribe**(`subscriber`): [`Subscription`](../@vicinae/namespaces/Cache/type-aliases/Subscription.md)

Registers a new subscriber that gets notified when cache data is set or removed.

#### Parameters

##### subscriber

[`Subscriber`](../@vicinae/namespaces/Cache/type-aliases/Subscriber.md)

#### Returns

[`Subscription`](../@vicinae/namespaces/Cache/type-aliases/Subscription.md)

a function that can be called to remove the subscriber.
