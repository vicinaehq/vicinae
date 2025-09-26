[**@vicinae/api**](../../../../README.md)

***

[@vicinae/api](../../../../README.md) / [OAuth](../README.md) / TokenSetOptions

# Interface: TokenSetOptions

Options for a [OAuth.TokenSet](../../../../variables/OAuth.md#tokenset) to store via OAuth.PKCEClient.setTokens.

## Properties

### accessToken

> **accessToken**: `string`

The access token returned by an OAuth token request.

***

### expiresIn?

> `optional` **expiresIn**: `number`

An optional expires value (in seconds) returned by an OAuth token request.

***

### idToken?

> `optional` **idToken**: `string`

An optional id token returned by an identity request (e.g. /me, Open ID Connect).

***

### refreshToken?

> `optional` **refreshToken**: `string`

An optional refresh token returned by an OAuth token request.

***

### scope?

> `optional` **scope**: `string`

The optional scope value returned by an OAuth token request.
