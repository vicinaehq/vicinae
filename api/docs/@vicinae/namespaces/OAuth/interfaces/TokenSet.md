[**@vicinae/api**](../../../../README.md)

***

[@vicinae/api](../../../../README.md) / [OAuth](../README.md) / TokenSet

# Interface: TokenSet

Defined in: [oauth.ts:169](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/oauth.ts#L169)

Describes the TokenSet created from an OAuth provider's token response.
The `accessToken` is the only required parameter but typically OAuth providers also return a refresh token, an expires value, and the scope.
Securely store a token set via OAuth.PKCEClient.setTokens and retrieve it via OAuth.PKCEClient.getTokens.

## Properties

### accessToken

> **accessToken**: `string`

Defined in: [oauth.ts:173](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/oauth.ts#L173)

The access token returned by an OAuth token request.

***

### expiresIn?

> `optional` **expiresIn**: `number`

Defined in: [oauth.ts:185](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/oauth.ts#L185)

An optional expires value (in seconds) returned by an OAuth token request.

***

### idToken?

> `optional` **idToken**: `string`

Defined in: [oauth.ts:181](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/oauth.ts#L181)

An optional id token returned by an identity request (e.g. /me, Open ID Connect).

***

### refreshToken?

> `optional` **refreshToken**: `string`

Defined in: [oauth.ts:177](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/oauth.ts#L177)

An optional refresh token returned by an OAuth token request.

***

### scope?

> `optional` **scope**: `string`

Defined in: [oauth.ts:191](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/oauth.ts#L191)

The optional space-delimited list of scopes returned by an OAuth token request.
You can use this to compare the currently stored access scopes against new access scopes the extension might require in a future version,
and then ask the user to re-authorize with new scopes.

***

### updatedAt

> **updatedAt**: `Date`

Defined in: [oauth.ts:195](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/oauth.ts#L195)

The date when the token set was stored via OAuth.PKCEClient.setTokens.

## Methods

### isExpired()

> **isExpired**(): `boolean`

Defined in: [oauth.ts:201](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/oauth.ts#L201)

A convenience method for checking whether the access token has expired.
The method factors in some seconds of "buffer", so it returns true a couple of seconds before the actual expiration time.
This requires the `expiresIn` parameter to be set.

#### Returns

`boolean`
