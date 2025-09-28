[**@vicinae/api**](../../../../README.md)

***

[@vicinae/api](../../../../README.md) / [OAuth](../README.md) / AuthorizationRequestOptions

# Interface: AuthorizationRequestOptions

The options for an authorization request via OAuth.PKCEClient.authorizationRequest.

## Properties

### clientId

> **clientId**: `string`

The client ID of the configured OAuth app.

***

### endpoint

> **endpoint**: `string`

The URL to the authorization endpoint for the OAuth provider.

***

### extraParameters?

> `optional` **extraParameters**: `Record`\<`string`, `string`\>

Optional additional parameters for the authorization request.
Note that some providers require additional parameters, for example to obtain long-lived refresh tokens.

***

### scope

> **scope**: `string`

A space-delimited list of scopes for identifying the resources to access on the user's behalf.
The scopes are typically shown to the user on the provider's consent screen in the browser.
Note that some providers require the same scopes be configured in the registered OAuth app.
