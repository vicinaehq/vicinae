[**@vicinae/api**](../README.md)

***

[@vicinae/api](../README.md) / showHUD

# Function: showHUD()

> **showHUD**(`title`, `options?`): `Promise`\<`void`\>

Close the window and show a small HUD where the window was previously opened.
Note that the HUD may not be shown in some environments.
Similarly to `closeWindow`, it is possible to override the pop to root behavior directly
by passing options to this function.

## Parameters

### title

`string`

### options?

#### clearRootSearch?

`boolean`

#### popToRootType?

[`PopToRootType`](../enumerations/PopToRootType.md)

## Returns

`Promise`\<`void`\>

## See

closeWindow
