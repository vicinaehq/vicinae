[**@vicinae/api**](../../../../README.md)

***

[@vicinae/api](../../../../README.md) / [OAuth](../README.md) / TokenSet

# Interface: TokenSet

Describes the TokenSet created from an OAuth provider's token response.
The `accessToken` is the only required parameter but typically OAuth providers also return a refresh token, an expires value, and the scope.
Securely store a token set via OAuth.PKCEClient.setTokens and retrieve it via OAuth.PKCEClient.getTokens.

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

The optional space-delimited list of scopes returned by an OAuth token request.
You can use this to compare the currently stored access scopes against new access scopes the extension might require in a future version,
and then ask the user to re-authorize with new scopes.

***

### updatedAt

> **updatedAt**: `Date`

The date when the token set was stored via OAuth.PKCEClient.setTokens.

## Methods

### isExpired()

> **isExpired**(): `boolean`

A convenience method for checking whether the access token has expired.
The method factors in some seconds of "buffer", so it returns true a couple of seconds before the actual expiration time.
This requires the `expiresIn` parameter to be set.

#### Returns

`boolean`
