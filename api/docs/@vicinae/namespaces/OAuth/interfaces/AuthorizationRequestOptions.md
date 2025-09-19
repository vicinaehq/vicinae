[**@vicinae/api**](../../../../README.md)

***

[@vicinae/api](../../../../README.md) / [OAuth](../README.md) / AuthorizationRequestOptions

# Interface: AuthorizationRequestOptions

Defined in: [oauth.ts:90](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/oauth.ts#L90)

The options for an authorization request via OAuth.PKCEClient.authorizationRequest.

## Properties

### clientId

> **clientId**: `string`

Defined in: [oauth.ts:98](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/oauth.ts#L98)

The client ID of the configured OAuth app.

***

### endpoint

> **endpoint**: `string`

Defined in: [oauth.ts:94](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/oauth.ts#L94)

The URL to the authorization endpoint for the OAuth provider.

***

### extraParameters?

> `optional` **extraParameters**: `Record`\<`string`, `string`\>

Defined in: [oauth.ts:109](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/oauth.ts#L109)

Optional additional parameters for the authorization request.
Note that some providers require additional parameters, for example to obtain long-lived refresh tokens.

***

### scope

> **scope**: `string`

Defined in: [oauth.ts:104](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/oauth.ts#L104)

A space-delimited list of scopes for identifying the resources to access on the user's behalf.
The scopes are typically shown to the user on the provider's consent screen in the browser.
Note that some providers require the same scopes be configured in the registered OAuth app.
