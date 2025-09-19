[**@vicinae/api**](../README.md)

***

[@vicinae/api](../README.md) / Message

# Type Alias: Message\<T\>

> **Message**\<`T`\> = `object`

Defined in: [bus.ts:8](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/bus.ts#L8)

## Type Parameters

### T

`T` = `Record`\<`string`, `any`\>

## Properties

### data

> **data**: `T`

Defined in: [bus.ts:15](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/bus.ts#L15)

***

### envelope

> **envelope**: `object`

Defined in: [bus.ts:9](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/bus.ts#L9)

#### action

> **action**: `string`

#### id

> **id**: `string`

#### type

> **type**: `"request"` \| `"response"` \| `"event"`

***

### error

> **error**: \{ `message?`: `string`; \} \| `null`

Defined in: [bus.ts:14](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/bus.ts#L14)
