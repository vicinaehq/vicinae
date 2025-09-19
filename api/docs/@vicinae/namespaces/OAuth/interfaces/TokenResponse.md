[**@vicinae/api**](../../../../README.md)

***

[@vicinae/api](../../../../README.md) / [OAuth](../README.md) / TokenResponse

# Interface: TokenResponse

Defined in: [oauth.ts:232](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/oauth.ts#L232)

Defines the standard JSON response for an OAuth token request.
The response can be directly used to store a [OAuth.TokenSet](../../../../variables/OAuth.md#tokenset) via OAuth.PKCEClient.setTokens.

## Properties

### access\_token

> **access\_token**: `string`

Defined in: [oauth.ts:236](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/oauth.ts#L236)

The `access_token` value returned by an OAuth token request.

***

### expires\_in?

> `optional` **expires\_in**: `number`

Defined in: [oauth.ts:248](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/oauth.ts#L248)

An optional `expires_in` value (in seconds) returned by an OAuth token request.

***

### id\_token?

> `optional` **id\_token**: `string`

Defined in: [oauth.ts:244](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/oauth.ts#L244)

An optional `id_token` value returned by an identity request (e.g. /me, Open ID Connect).

***

### refresh\_token?

> `optional` **refresh\_token**: `string`

Defined in: [oauth.ts:240](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/oauth.ts#L240)

An optional `refresh_token` value returned by an OAuth token request.

***

### scope?

> `optional` **scope**: `string`

Defined in: [oauth.ts:252](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/oauth.ts#L252)

The optional `scope` value returned by an OAuth token request.
