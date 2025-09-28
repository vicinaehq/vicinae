[**@vicinae/api**](../README.md)

***

[@vicinae/api](../README.md) / LocalStorage

# Class: LocalStorage

## Constructors

### Constructor

> **new LocalStorage**(): `LocalStorage`

#### Returns

`LocalStorage`

## Methods

### allItems()

> `static` **allItems**(): `Promise`\<[`Values`](../@vicinae/namespaces/LocalStorage/type-aliases/Values.md)\>

#### Returns

`Promise`\<[`Values`](../@vicinae/namespaces/LocalStorage/type-aliases/Values.md)\>

***

### clear()

> `static` **clear**(): `Promise`\<`void`\>

#### Returns

`Promise`\<`void`\>

***

### getItem()

> `static` **getItem**\<`T`\>(`key`): `Promise`\<`undefined` \| `T`\>

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

#### Parameters

##### key

`string`

#### Returns

`Promise`\<`void`\>

***

### setItem()

> `static` **setItem**(`key`, `value`): `Promise`\<`void`\>

#### Parameters

##### key

`string`

##### value

[`Value`](../@vicinae/namespaces/LocalStorage/type-aliases/Value.md)

#### Returns

`Promise`\<`void`\>
