[**@vicinae/api**](../../../../README.md)

***

[@vicinae/api](../../../../README.md) / [OAuth](../README.md) / AuthorizationRequestURLParams

# Interface: AuthorizationRequestURLParams

Values of [OAuth.AuthorizationRequest](AuthorizationRequest.md).
The PKCE client automatically generates the values for you and returns them for OAuth.PKCEClient.authorizationRequest.

## Extended by

- [`AuthorizationRequest`](AuthorizationRequest.md)

## Properties

### codeChallenge

> **codeChallenge**: `string`

The PKCE `code_challenge` value.

***

### codeVerifier

> **codeVerifier**: `string`

The PKCE `code_verifier` value.

***

### redirectURI

> **redirectURI**: `string`

The OAuth `redirect_uri` value.

***

### state

> **state**: `string`

The OAuth `state` value.
