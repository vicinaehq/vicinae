[**@vicinae/api**](../../../../README.md)

***

[@vicinae/api](../../../../README.md) / [Toast](../README.md) / Options

# Interface: Options

Defined in: [toast.ts:195](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/toast.ts#L195)

The options to create a [Toast](../../../../classes/Toast.md).

## Example

```typescript
import { showToast, Toast } from "@raycast/api";

export default async () => {
  const options: Toast.Options = {
    style: Toast.Style.Success,
    title: "Finished cooking",
    message: "Delicious pasta for lunch",
    primaryAction: {
      title: 'Do something',
      onAction: () => {
        console.log("The toast action has been triggered")
      }
    }
  };
  await showToast(options);
};
```

## Extended by

- [`ToastOptions`](../../../../interfaces/ToastOptions.md)

## Properties

### message?

> `optional` **message**: `string`

Defined in: [toast.ts:203](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/toast.ts#L203)

An additional message for the Toast. Useful to show more information, e.g. an identifier of a newly created asset.

***

### primaryAction?

> `optional` **primaryAction**: [`ActionOptions`](ActionOptions.md)

Defined in: [toast.ts:211](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/toast.ts#L211)

The primary Action the user can take when hovering on the Toast.

***

### secondaryAction?

> `optional` **secondaryAction**: [`ActionOptions`](ActionOptions.md)

Defined in: [toast.ts:215](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/toast.ts#L215)

The secondary Action the user can take when hovering on the Toast.

***

### style?

> `optional` **style**: [`Style`](../enumerations/Style.md)

Defined in: [toast.ts:207](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/toast.ts#L207)

The style of a Toast.

***

### title

> **title**: `string`

Defined in: [toast.ts:199](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/toast.ts#L199)

The title of a Toast. Displayed on the top.
