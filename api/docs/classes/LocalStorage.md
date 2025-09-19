[**@vicinae/api**](../README.md)

***

[@vicinae/api](../README.md) / LocalStorage

# Class: LocalStorage

Defined in: [local-storage.ts:5](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/local-storage.ts#L5)

## Constructors

### Constructor

> **new LocalStorage**(): `LocalStorage`

#### Returns

`LocalStorage`

## Methods

### allItems()

> `static` **allItems**(): `Promise`\<[`Values`](../@vicinae/namespaces/LocalStorage/type-aliases/Values.md)\>

Defined in: [local-storage.ts:31](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/local-storage.ts#L31)

#### Returns

`Promise`\<[`Values`](../@vicinae/namespaces/LocalStorage/type-aliases/Values.md)\>

***

### clear()

> `static` **clear**(): `Promise`\<`void`\>

Defined in: [local-storage.ts:39](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/local-storage.ts#L39)

#### Returns

`Promise`\<`void`\>

***

### getItem()

> `static` **getItem**\<`T`\>(`key`): `Promise`\<`undefined` \| `T`\>

Defined in: [local-storage.ts:11](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/local-storage.ts#L11)

#### Type Parameters

##### T

`T` *extends* [`Value`](../@vicinae/namespaces/LocalStorage/type-aliases/Value.md)

#### Parameters

##### key

`string`

#### Returns

`Promise`\<`undefined` \| `T`\>

***

### removeItem()

> `static` **removeItem**(`key`): `Promise`\<`void`\>

Defined in: [local-storage.ts:27](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/local-storage.ts#L27)

#### Parameters

##### key

`string`

#### Returns

`Promise`\<`void`\>

***

### setItem()

> `static` **setItem**(`key`, `value`): `Promise`\<`void`\>

Defined in: [local-storage.ts:23](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/local-storage.ts#L23)

#### Parameters

##### key

`string`

##### value

[`Value`](../@vicinae/namespaces/LocalStorage/type-aliases/Value.md)

#### Returns

`Promise`\<`void`\>
