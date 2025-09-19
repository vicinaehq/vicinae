[**@vicinae/api**](../../../../../../README.md)

***

[@vicinae/api](../../../../../../README.md) / [OAuth](../../../README.md) / [PKCEClient](../README.md) / Options

# Interface: Options

Defined in: [oauth.ts:34](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/oauth.ts#L34)

The options for creating a new [OAuth.PKCEClient](../../../../../../variables/OAuth.md#pkceclient).

## Properties

### description?

> `optional` **description**: `string`

Defined in: [oauth.ts:59](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/oauth.ts#L59)

An optional description, shown in the OAuth overlay.
You can use this to customize the message for the end user, for example for handling scope changes or other migrations.
Raycast shows a default message if this is not configured.

***

### providerIcon?

> `optional` **providerIcon**: [`ImageLike`](../../../../Image/type-aliases/ImageLike.md)

Defined in: [oauth.ts:48](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/oauth.ts#L48)

An icon displayed in the OAuth overlay.
Make sure to provide at least a size of 64x64 pixels.

***

### providerId?

> `optional` **providerId**: `string`

Defined in: [oauth.ts:53](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/oauth.ts#L53)

An optional ID for associating the client with a provider.
Only set this if you use multiple different clients in your extension.

***

### providerName

> **providerName**: `string`

Defined in: [oauth.ts:43](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/oauth.ts#L43)

The name of the provider, displayed in the OAuth overlay.

***

### redirectMethod

> **redirectMethod**: `OauthRedirectMethod`

Defined in: [oauth.ts:39](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/oauth.ts#L39)

The redirect method for the OAuth flow.
Make sure to set this to the correct method for the provider, see [OAuth.RedirectMethod](../../../../../../variables/OAuth.md#redirectmethod) for more information.
