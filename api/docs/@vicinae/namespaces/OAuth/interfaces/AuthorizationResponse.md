[**@vicinae/api**](../../../../README.md)

***

[@vicinae/api](../../../../README.md) / [OAuth](../README.md) / AuthorizationResponse

# Interface: AuthorizationResponse

The response returned by OAuth.PKCEClient.authorize, containing the authorization code after the provider redirect.
You can then exchange the authorization code for an access token using the provider's token endpoint.

## Properties

### authorizationCode

> **authorizationCode**: `string`

The authorization code from the OAuth provider.
