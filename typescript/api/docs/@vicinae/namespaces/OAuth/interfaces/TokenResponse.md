[**@vicinae/api**](../../../../README.md)

***

[@vicinae/api](../../../../README.md) / [OAuth](../README.md) / TokenResponse

# Interface: TokenResponse

Defines the standard JSON response for an OAuth token request.
The response can be directly used to store a [OAuth.TokenSet](../../../../variables/OAuth.md#tokenset) via OAuth.PKCEClient.setTokens.

## Properties

### access\_token

> **access\_token**: `string`

The `access_token` value returned by an OAuth token request.

***

### expires\_in?

> `optional` **expires\_in**: `number`

An optional `expires_in` value (in seconds) returned by an OAuth token request.

***

### id\_token?

> `optional` **id\_token**: `string`

An optional `id_token` value returned by an identity request (e.g. /me, Open ID Connect).

***

### refresh\_token?

> `optional` **refresh\_token**: `string`

An optional `refresh_token` value returned by an OAuth token request.

***

### scope?

> `optional` **scope**: `string`

The optional `scope` value returned by an OAuth token request.
