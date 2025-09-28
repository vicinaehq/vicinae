[**@vicinae/api**](../../../../README.md)

***

[@vicinae/api](../../../../README.md) / [Toast](../README.md) / Options

# Interface: Options

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

An additional message for the Toast. Useful to show more information, e.g. an identifier of a newly created asset.

***

### primaryAction?

> `optional` **primaryAction**: [`ActionOptions`](ActionOptions.md)

The primary Action the user can take when hovering on the Toast.

***

### secondaryAction?

> `optional` **secondaryAction**: [`ActionOptions`](ActionOptions.md)

The secondary Action the user can take when hovering on the Toast.

***

### style?

> `optional` **style**: [`Style`](../enumerations/Style.md)

The style of a Toast.

***

### title

> **title**: `string`

The title of a Toast. Displayed on the top.
