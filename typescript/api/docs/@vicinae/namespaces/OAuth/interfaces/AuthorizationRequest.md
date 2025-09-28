[**@vicinae/api**](../../../../README.md)

***

[@vicinae/api](../../../../README.md) / [OAuth](../README.md) / AuthorizationRequest

# Interface: AuthorizationRequest

The request returned by OAuth.PKCEClient.authorizationRequest.
Can be used as direct input to OAuth.PKCEClient.authorize, or
to extract parameters for constructing a custom URL in [OAuth.AuthorizationOptions](AuthorizationOptions.md).

## Extends

- [`AuthorizationRequestURLParams`](AuthorizationRequestURLParams.md)

## Properties

### codeChallenge

> **codeChallenge**: `string`

The PKCE `code_challenge` value.

#### Inherited from

[`AuthorizationRequestURLParams`](AuthorizationRequestURLParams.md).[`codeChallenge`](AuthorizationRequestURLParams.md#codechallenge)

***

### codeVerifier

> **codeVerifier**: `string`

The PKCE `code_verifier` value.

#### Inherited from

[`AuthorizationRequestURLParams`](AuthorizationRequestURLParams.md).[`codeVerifier`](AuthorizationRequestURLParams.md#codeverifier)

***

### redirectURI

> **redirectURI**: `string`

The OAuth `redirect_uri` value.

#### Inherited from

[`AuthorizationRequestURLParams`](AuthorizationRequestURLParams.md).[`redirectURI`](AuthorizationRequestURLParams.md#redirecturi)

***

### state

> **state**: `string`

The OAuth `state` value.

#### Inherited from

[`AuthorizationRequestURLParams`](AuthorizationRequestURLParams.md).[`state`](AuthorizationRequestURLParams.md#state)

## Methods

### toURL()

> **toURL**(): `string`

Constructs the full authorization URL.

#### Returns

`string`
