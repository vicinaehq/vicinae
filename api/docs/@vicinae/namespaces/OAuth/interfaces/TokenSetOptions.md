[**@vicinae/api**](../../../../README.md)

***

[@vicinae/api](../../../../README.md) / [OAuth](../README.md) / TokenSetOptions

# Interface: TokenSetOptions

Defined in: [oauth.ts:206](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/oauth.ts#L206)

Options for a [OAuth.TokenSet](../../../../variables/OAuth.md#tokenset) to store via OAuth.PKCEClient.setTokens.

## Properties

### accessToken

> **accessToken**: `string`

Defined in: [oauth.ts:210](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/oauth.ts#L210)

The access token returned by an OAuth token request.

***

### expiresIn?

> `optional` **expiresIn**: `number`

Defined in: [oauth.ts:222](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/oauth.ts#L222)

An optional expires value (in seconds) returned by an OAuth token request.

***

### idToken?

> `optional` **idToken**: `string`

Defined in: [oauth.ts:218](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/oauth.ts#L218)

An optional id token returned by an identity request (e.g. /me, Open ID Connect).

***

### refreshToken?

> `optional` **refreshToken**: `string`

Defined in: [oauth.ts:214](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/oauth.ts#L214)

An optional refresh token returned by an OAuth token request.

***

### scope?

> `optional` **scope**: `string`

Defined in: [oauth.ts:226](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/oauth.ts#L226)

The optional scope value returned by an OAuth token request.
