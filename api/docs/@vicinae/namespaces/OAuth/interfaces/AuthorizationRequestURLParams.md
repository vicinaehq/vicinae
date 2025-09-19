[**@vicinae/api**](../../../../README.md)

***

[@vicinae/api](../../../../README.md) / [OAuth](../README.md) / AuthorizationRequestURLParams

# Interface: AuthorizationRequestURLParams

Defined in: [oauth.ts:115](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/oauth.ts#L115)

Values of [OAuth.AuthorizationRequest](AuthorizationRequest.md).
The PKCE client automatically generates the values for you and returns them for OAuth.PKCEClient.authorizationRequest.

## Extended by

- [`AuthorizationRequest`](AuthorizationRequest.md)

## Properties

### codeChallenge

> **codeChallenge**: `string`

Defined in: [oauth.ts:119](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/oauth.ts#L119)

The PKCE `code_challenge` value.

***

### codeVerifier

> **codeVerifier**: `string`

Defined in: [oauth.ts:123](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/oauth.ts#L123)

The PKCE `code_verifier` value.

***

### redirectURI

> **redirectURI**: `string`

Defined in: [oauth.ts:131](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/oauth.ts#L131)

The OAuth `redirect_uri` value.

***

### state

> **state**: `string`

Defined in: [oauth.ts:127](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/oauth.ts#L127)

The OAuth `state` value.
