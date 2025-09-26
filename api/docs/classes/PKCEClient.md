[**@vicinae/api**](../README.md)

***

[@vicinae/api](../README.md) / PKCEClient

# Class: PKCEClient

## Constructors

### Constructor

> **new PKCEClient**(`options`): `PKCEClient`

#### Parameters

##### options

[`Options`](../@vicinae/namespaces/OAuth/namespaces/PKCEClient/interfaces/Options.md)

#### Returns

`PKCEClient`

## Properties

### description?

> `optional` **description**: `string`

***

### providerIcon?

> `optional` **providerIcon**: [`ImageLike`](../@vicinae/namespaces/Image/type-aliases/ImageLike.md)

***

### providerId?

> `optional` **providerId**: `string`

***

### providerName

> **providerName**: `string`

***

### redirectMethod

> **redirectMethod**: `OauthRedirectMethod`

## Methods

### authorizationRequest()

> **authorizationRequest**(`options`): `Promise`\<[`AuthorizationRequest`](../@vicinae/namespaces/OAuth/interfaces/AuthorizationRequest.md)\>

Creates an authorization request for the provided authorization endpoint, client ID, and scopes.
You need to first create the authorization request before calling OAuth.PKCEClient.authorize.

#### Parameters

##### options

[`AuthorizationRequestOptions`](../@vicinae/namespaces/OAuth/interfaces/AuthorizationRequestOptions.md)

#### Returns

`Promise`\<[`AuthorizationRequest`](../@vicinae/namespaces/OAuth/interfaces/AuthorizationRequest.md)\>

A promise for an [OAuth.AuthorizationRequest](../@vicinae/namespaces/OAuth/interfaces/AuthorizationRequest.md) that you can use as input for OAuth.PKCEClient.authorize.

#### Remarks

The generated code challenge for the PKCE request uses the S256 method.

***

### authorize()

> **authorize**(`options`): `Promise`\<[`AuthorizationResponse`](../@vicinae/namespaces/OAuth/interfaces/AuthorizationResponse.md)\>

Starts the authorization and shows the OAuth overlay in Raycast.
As parameter you can either directly use the returned request from OAuth.PKCEClient.authorizationRequest,
or customize the URL by extracting parameters from [OAuth.AuthorizationRequest](../@vicinae/namespaces/OAuth/interfaces/AuthorizationRequest.md) and providing your own URL via AuthorizationOptions.
Eventually the URL will be used to open the authorization page of the provider in the web browser.

#### Parameters

##### options

[`AuthorizationRequest`](../@vicinae/namespaces/OAuth/interfaces/AuthorizationRequest.md) | [`AuthorizationOptions`](../@vicinae/namespaces/OAuth/interfaces/AuthorizationOptions.md)

#### Returns

`Promise`\<[`AuthorizationResponse`](../@vicinae/namespaces/OAuth/interfaces/AuthorizationResponse.md)\>

A promise for an [OAuth.AuthorizationResponse](../@vicinae/namespaces/OAuth/interfaces/AuthorizationResponse.md), which contains the authorization code needed for the token exchange.
The promise is resolved when the user was redirected back from the provider's authorization page to the Raycast extension.

***

### buildAuthUrl()

> **buildAuthUrl**(`options`, `state`, `codeChallenge`, `redirectURI`): `string`

#### Parameters

##### options

[`AuthorizationRequestOptions`](../@vicinae/namespaces/OAuth/interfaces/AuthorizationRequestOptions.md)

##### state

`string`

##### codeChallenge

`string`

##### redirectURI

`string`

#### Returns

`string`

***

### getRedirectURI()

> **getRedirectURI**(): `"https://raycast.com/redirect?packageName=Extension"` \| `"raycast://oauth?package_name=Extension"` \| `"com.raycast:/oauth?package_name=Extension"`

#### Returns

`"https://raycast.com/redirect?packageName=Extension"` \| `"raycast://oauth?package_name=Extension"` \| `"com.raycast:/oauth?package_name=Extension"`

***

### getTokens()

> **getTokens**(): `Promise`\<`undefined` \| [`TokenSet`](../@vicinae/namespaces/OAuth/interfaces/TokenSet.md)\>

Retrieves the stored [OAuth.TokenSet](../variables/OAuth.md#tokenset) for the client.
You can use this to initially check whether the authorization flow should be initiated or
the user is already logged in and you might have to refresh the access token.

#### Returns

`Promise`\<`undefined` \| [`TokenSet`](../@vicinae/namespaces/OAuth/interfaces/TokenSet.md)\>

A promise that resolves when the token set has been retrieved.

***

### removeTokens()

> **removeTokens**(): `Promise`\<`void`\>

Removes the stored [OAuth.TokenSet](../variables/OAuth.md#tokenset) for the client.

#### Returns

`Promise`\<`void`\>

#### Remarks

Raycast automatically shows a logout preference that removes the token set.
Use this method only if you need to provide an additional logout option in your extension or you want to remove the token set because of a migration.

***

### setTokens()

> **setTokens**(`options`): `Promise`\<`void`\>

Securely stores a [OAuth.TokenSet](../variables/OAuth.md#tokenset) for the provider. Use this after fetching the access token from the provider.
If the provider returns a a standard OAuth JSON token response, you can directly pass the [OAuth.TokenResponse](../@vicinae/namespaces/OAuth/interfaces/TokenResponse.md).
At a minimum, you need to set the [OAuth.TokenSet.accessToken](../@vicinae/namespaces/OAuth/interfaces/TokenSet.md#accesstoken), and typically you also set [OAuth.TokenSet.refreshToken](../@vicinae/namespaces/OAuth/interfaces/TokenSet.md#refreshtoken) and [OAuth.TokenSet.isExpired](../@vicinae/namespaces/OAuth/interfaces/TokenSet.md#isexpired).
Raycast automatically shows a logout preference for the extension when a token set was saved.

#### Parameters

##### options

[`TokenSetOptions`](../@vicinae/namespaces/OAuth/interfaces/TokenSetOptions.md) | [`TokenResponse`](../@vicinae/namespaces/OAuth/interfaces/TokenResponse.md)

#### Returns

`Promise`\<`void`\>

A promise that resolves when the token set has been stored.

#### Remarks

If you want to make use of the convenience [OAuth.TokenSet.isExpired](../@vicinae/namespaces/OAuth/interfaces/TokenSet.md#isexpired) method, the property [OAuth.TokenSet.expiresIn](../@vicinae/namespaces/OAuth/interfaces/TokenSet.md#expiresin) must be configured.
