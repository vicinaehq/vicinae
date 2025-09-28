[**@vicinae/api**](../README.md)

***

[@vicinae/api](../README.md) / Message

# Type Alias: Message\<T\>

> **Message**\<`T`\> = `object`

## Type Parameters

### T

`T` = `Record`\<`string`, `any`\>

## Properties

### data

> **data**: `T`

***

### envelope

> **envelope**: `object`

#### action

> **action**: `string`

#### id

> **id**: `string`

#### type

> **type**: `"request"` \| `"response"` \| `"event"`

***

### error

> **error**: \{ `message?`: `string`; \} \| `null`
