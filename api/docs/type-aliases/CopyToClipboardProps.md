[**@vicinae/api**](../README.md)

***

[@vicinae/api](../README.md) / CopyToClipboardProps

# Type Alias: CopyToClipboardProps

> **CopyToClipboardProps** = `Omit`\<[`BaseActionProps`](BaseActionProps.md), `"title"`\> & `object`

Defined in: [components/actions.tsx:23](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/components/actions.tsx#L23)

## Type Declaration

### concealed?

> `optional` **concealed**: `boolean`

### content

> **content**: `string`

### onCopy()?

> `optional` **onCopy**: (`content`) => `void`

#### Parameters

##### content

`string` | `number` | [`Content`](../@vicinae/namespaces/Clipboard/type-aliases/Content.md)

#### Returns

`void`

### title?

> `optional` **title**: `string`
