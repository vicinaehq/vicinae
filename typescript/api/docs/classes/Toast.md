[**@vicinae/api**](../README.md)

***

[@vicinae/api](../README.md) / Toast

# Class: Toast

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

An additional message for the Toast. Useful to show more information, e.g. an identifier of a newly created asset.

##### Returns

`undefined` \| `string`

#### Set Signature

> **set** **message**(`message`): `void`

##### Parameters

###### message

`undefined` | `string`

##### Returns

`void`

***

### primaryAction

#### Get Signature

> **get** **primaryAction**(): `undefined` \| [`ActionOptions`](../@vicinae/namespaces/Toast/interfaces/ActionOptions.md)

The primary Action the user can take when hovering on the Toast.

##### Returns

`undefined` \| [`ActionOptions`](../@vicinae/namespaces/Toast/interfaces/ActionOptions.md)

#### Set Signature

> **set** **primaryAction**(`action`): `void`

##### Parameters

###### action

`undefined` | [`ActionOptions`](../@vicinae/namespaces/Toast/interfaces/ActionOptions.md)

##### Returns

`void`

***

### secondaryAction

#### Get Signature

> **get** **secondaryAction**(): `undefined` \| [`ActionOptions`](../@vicinae/namespaces/Toast/interfaces/ActionOptions.md)

The secondary Action the user can take when hovering on the Toast.

##### Returns

`undefined` \| [`ActionOptions`](../@vicinae/namespaces/Toast/interfaces/ActionOptions.md)

#### Set Signature

> **set** **secondaryAction**(`action`): `void`

##### Parameters

###### action

`undefined` | [`ActionOptions`](../@vicinae/namespaces/Toast/interfaces/ActionOptions.md)

##### Returns

`void`

***

### style

#### Get Signature

> **get** **style**(): [`Style`](../@vicinae/namespaces/Toast/enumerations/Style.md)

The style of a Toast.

##### Returns

[`Style`](../@vicinae/namespaces/Toast/enumerations/Style.md)

#### Set Signature

> **set** **style**(`style`): `void`

##### Parameters

###### style

[`Style`](../@vicinae/namespaces/Toast/enumerations/Style.md)

##### Returns

`void`

***

### title

#### Get Signature

> **get** **title**(): `string`

The title of a Toast. Displayed on the top.

##### Returns

`string`

#### Set Signature

> **set** **title**(`title`): `void`

##### Parameters

###### title

`string`

##### Returns

`void`

## Methods

### hide()

> **hide**(): `Promise`\<`void`\>

Hides the Toast.

#### Returns

`Promise`\<`void`\>

A Promise that resolves when toast is hidden.

***

### show()

> **show**(): `Promise`\<`void`\>

Shows the Toast.

#### Returns

`Promise`\<`void`\>

A Promise that resolves when the toast is shown.
