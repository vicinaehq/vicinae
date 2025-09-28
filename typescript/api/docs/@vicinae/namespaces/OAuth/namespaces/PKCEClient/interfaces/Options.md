[**@vicinae/api**](../../../../../../README.md)

***

[@vicinae/api](../../../../../../README.md) / [OAuth](../../../README.md) / [PKCEClient](../README.md) / Options

# Interface: Options

The options for creating a new [OAuth.PKCEClient](../../../../../../variables/OAuth.md#pkceclient).

## Properties

### description?

> `optional` **description**: `string`

An optional description, shown in the OAuth overlay.
You can use this to customize the message for the end user, for example for handling scope changes or other migrations.
Raycast shows a default message if this is not configured.

***

### providerIcon?

> `optional` **providerIcon**: [`ImageLike`](../../../../Image/type-aliases/ImageLike.md)

An icon displayed in the OAuth overlay.
Make sure to provide at least a size of 64x64 pixels.

***

### providerId?

> `optional` **providerId**: `string`

An optional ID for associating the client with a provider.
Only set this if you use multiple different clients in your extension.

***

### providerName

> **providerName**: `string`

The name of the provider, displayed in the OAuth overlay.

***

### redirectMethod

> **redirectMethod**: `OauthRedirectMethod`

The redirect method for the OAuth flow.
Make sure to set this to the correct method for the provider, see [OAuth.RedirectMethod](../../../../../../variables/OAuth.md#redirectmethod) for more information.
