[**@vicinae/api**](../../../../README.md)

***

[@vicinae/api](../../../../README.md) / [OAuth](../README.md) / AuthorizationResponse

# Interface: AuthorizationResponse

Defined in: [oauth.ts:158](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/oauth.ts#L158)

The response returned by OAuth.PKCEClient.authorize, containing the authorization code after the provider redirect.
You can then exchange the authorization code for an access token using the provider's token endpoint.

## Properties

### authorizationCode

> **authorizationCode**: `string`

Defined in: [oauth.ts:162](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/oauth.ts#L162)

The authorization code from the OAuth provider.
