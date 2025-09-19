[**@vicinae/api**](../../../../README.md)

***

[@vicinae/api](../../../../README.md) / [Toast](../README.md) / ActionOptions

# Interface: ActionOptions

Defined in: [toast.ts:220](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/toast.ts#L220)

The options to create a [Toast](../../../../classes/Toast.md) Action.

## Extended by

- [`ToastActionOptions`](../../../../interfaces/ToastActionOptions.md)

## Properties

### onAction()

> **onAction**: (`toast`) => `void`

Defined in: [toast.ts:232](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/toast.ts#L232)

A callback called when the action is triggered.

#### Parameters

##### toast

[`Toast`](../../../../classes/Toast.md)

#### Returns

`void`

***

### shortcut?

> `optional` **shortcut**: [`Shortcut`](../../Keyboard/type-aliases/Shortcut.md)

Defined in: [toast.ts:228](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/toast.ts#L228)

The keyboard shortcut for the action.

***

### title

> **title**: `string`

Defined in: [toast.ts:224](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/toast.ts#L224)

The title of the action.
