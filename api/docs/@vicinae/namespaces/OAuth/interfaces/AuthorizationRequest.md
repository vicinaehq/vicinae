[**@vicinae/api**](../../../../README.md)

***

[@vicinae/api](../../../../README.md) / [OAuth](../README.md) / AuthorizationRequest

# Interface: AuthorizationRequest

Defined in: [oauth.ts:138](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/oauth.ts#L138)

The request returned by OAuth.PKCEClient.authorizationRequest.
Can be used as direct input to OAuth.PKCEClient.authorize, or
to extract parameters for constructing a custom URL in [OAuth.AuthorizationOptions](AuthorizationOptions.md).

## Extends

- [`AuthorizationRequestURLParams`](AuthorizationRequestURLParams.md)

## Properties

### codeChallenge

> **codeChallenge**: `string`

Defined in: [oauth.ts:119](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/oauth.ts#L119)

The PKCE `code_challenge` value.

#### Inherited from

[`AuthorizationRequestURLParams`](AuthorizationRequestURLParams.md).[`codeChallenge`](AuthorizationRequestURLParams.md#codechallenge)

***

### codeVerifier

> **codeVerifier**: `string`

Defined in: [oauth.ts:123](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/oauth.ts#L123)

The PKCE `code_verifier` value.

#### Inherited from

[`AuthorizationRequestURLParams`](AuthorizationRequestURLParams.md).[`codeVerifier`](AuthorizationRequestURLParams.md#codeverifier)

***

### redirectURI

> **redirectURI**: `string`

Defined in: [oauth.ts:131](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/oauth.ts#L131)

The OAuth `redirect_uri` value.

#### Inherited from

[`AuthorizationRequestURLParams`](AuthorizationRequestURLParams.md).[`redirectURI`](AuthorizationRequestURLParams.md#redirecturi)

***

### state

> **state**: `string`

Defined in: [oauth.ts:127](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/oauth.ts#L127)

The OAuth `state` value.

#### Inherited from

[`AuthorizationRequestURLParams`](AuthorizationRequestURLParams.md).[`state`](AuthorizationRequestURLParams.md#state)

## Methods

### toURL()

> **toURL**(): `string`

Defined in: [oauth.ts:142](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/oauth.ts#L142)

Constructs the full authorization URL.

#### Returns

`string`
