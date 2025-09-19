[**@vicinae/api**](../README.md)

***

[@vicinae/api](../README.md) / Toast

# Class: Toast

Defined in: [toast.ts:26](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/toast.ts#L26)

A Toast with a certain style, title, and message.

## Example

```typescript
import { showToast, Toast } from "@raycast/api";
import { setTimeout } from "timers/promises";

export default async () => {
  const toast = await showToast({ style: Toast.Style.Animated, title: "Uploading image" });

  await setTimeout(1000);

  toast.style = Toast.Style.Success;
  toast.title = "Uploaded image";
};
```

## Constructors

### Constructor

> **new Toast**(`props`): `Toast`

Defined in: [toast.ts:50](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/toast.ts#L50)

Deprecated - Use `showToast` instead

#### Parameters

##### props

[`Options`](../@vicinae/namespaces/Toast/interfaces/Options.md)

#### Returns

`Toast`

## Accessors

### message

#### Get Signature

> **get** **message**(): `undefined` \| `string`

Defined in: [toast.ts:96](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/toast.ts#L96)

An additional message for the Toast. Useful to show more information, e.g. an identifier of a newly created asset.

##### Returns

`undefined` \| `string`

#### Set Signature

> **set** **message**(`message`): `void`

Defined in: [toast.ts:99](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/toast.ts#L99)

##### Parameters

###### message

`undefined` | `string`

##### Returns

`void`

***

### primaryAction

#### Get Signature

> **get** **primaryAction**(): `undefined` \| [`ActionOptions`](../@vicinae/namespaces/Toast/interfaces/ActionOptions.md)

Defined in: [toast.ts:105](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/toast.ts#L105)

The primary Action the user can take when hovering on the Toast.

##### Returns

`undefined` \| [`ActionOptions`](../@vicinae/namespaces/Toast/interfaces/ActionOptions.md)

#### Set Signature

> **set** **primaryAction**(`action`): `void`

Defined in: [toast.ts:108](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/toast.ts#L108)

##### Parameters

###### action

`undefined` | [`ActionOptions`](../@vicinae/namespaces/Toast/interfaces/ActionOptions.md)

##### Returns

`void`

***

### secondaryAction

#### Get Signature

> **get** **secondaryAction**(): `undefined` \| [`ActionOptions`](../@vicinae/namespaces/Toast/interfaces/ActionOptions.md)

Defined in: [toast.ts:114](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/toast.ts#L114)

The secondary Action the user can take when hovering on the Toast.

##### Returns

`undefined` \| [`ActionOptions`](../@vicinae/namespaces/Toast/interfaces/ActionOptions.md)

#### Set Signature

> **set** **secondaryAction**(`action`): `void`

Defined in: [toast.ts:117](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/toast.ts#L117)

##### Parameters

###### action

`undefined` | [`ActionOptions`](../@vicinae/namespaces/Toast/interfaces/ActionOptions.md)

##### Returns

`void`

***

### style

#### Get Signature

> **get** **style**(): [`Style`](../@vicinae/namespaces/Toast/enumerations/Style.md)

Defined in: [toast.ts:78](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/toast.ts#L78)

The style of a Toast.

##### Returns

[`Style`](../@vicinae/namespaces/Toast/enumerations/Style.md)

#### Set Signature

> **set** **style**(`style`): `void`

Defined in: [toast.ts:81](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/toast.ts#L81)

##### Parameters

###### style

[`Style`](../@vicinae/namespaces/Toast/enumerations/Style.md)

##### Returns

`void`

***

### title

#### Get Signature

> **get** **title**(): `string`

Defined in: [toast.ts:87](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/toast.ts#L87)

The title of a Toast. Displayed on the top.

##### Returns

`string`

#### Set Signature

> **set** **title**(`title`): `void`

Defined in: [toast.ts:90](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/toast.ts#L90)

##### Parameters

###### title

`string`

##### Returns

`void`

## Methods

### hide()

> **hide**(): `Promise`\<`void`\>

Defined in: [toast.ts:164](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/toast.ts#L164)

Hides the Toast.

#### Returns

`Promise`\<`void`\>

A Promise that resolves when toast is hidden.

***

### show()

> **show**(): `Promise`\<`void`\>

Defined in: [toast.ts:125](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/toast.ts#L125)

Shows the Toast.

#### Returns

`Promise`\<`void`\>

A Promise that resolves when the toast is shown.
