[**@vicinae/api**](../README.md)

***

[@vicinae/api](../README.md) / Environment

# Interface: Environment

Defined in: [environment.ts:56](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/environment.ts#L56)

## Properties

### appearance

> **appearance**: `"light"` \| `"dark"`

Defined in: [environment.ts:91](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/environment.ts#L91)

The appearance used by the Raycast application.

***

### assetsPath

> **assetsPath**: `string`

Defined in: [environment.ts:79](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/environment.ts#L79)

The absolute path to the assets directory of the extension.

#### Remarks

This directory is used internally to load icons, images, and other bundled assets.

***

### commandMode

> **commandMode**: `"menu-bar"` \| `"no-view"` \| `"view"`

Defined in: [environment.ts:72](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/environment.ts#L72)

The mode of the launched command, as specified in package.json

***

### commandName

> **commandName**: `string`

Defined in: [environment.ts:68](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/environment.ts#L68)

The name of the launched command, as specified in package.json

***

### extensionName

> **extensionName**: `string`

Defined in: [environment.ts:64](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/environment.ts#L64)

The name of the extension, as specified in package.json

***

### isDevelopment

> **isDevelopment**: `boolean`

Defined in: [environment.ts:87](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/environment.ts#L87)

Indicates whether the command is a development command (vs. an installed command from the Store).

***

### isRaycast

> **isRaycast**: `boolean`

Defined in: [environment.ts:137](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/environment.ts#L137)

Whether we run an actual Raycast extension in compatibility mode.
This is used internally to provide Raycast-compatible interfaces.

***

### ~~launchContext?~~

> `optional` **launchContext**: [`LaunchContext`](LaunchContext.md)

Defined in: [environment.ts:123](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/environment.ts#L123)

#### Deprecated

Use the top-level prop `launchContext` instead.

***

### launchType

> **launchType**: [`LaunchType`](../enumerations/LaunchType.md)

Defined in: [environment.ts:104](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/environment.ts#L104)

The type of launch for the command (user initiated or background).

***

### raycastVersion

> **raycastVersion**: `string`

Defined in: [environment.ts:60](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/environment.ts#L60)

The version of the main Raycast app

***

### supportPath

> **supportPath**: `string`

Defined in: [environment.ts:83](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/environment.ts#L83)

The absolute path for the support directory of an extension. Use it to read and write files related to your extension or command.

***

### textSize

> **textSize**: `"medium"` \| `"large"`

Defined in: [environment.ts:100](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/environment.ts#L100)

The text size used by the Raycast application.

***

### ~~theme~~

> **theme**: `"light"` \| `"dark"`

Defined in: [environment.ts:96](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/environment.ts#L96)

The theme used by the Raycast application.

#### Deprecated

Use `appearance` instead

***

### vicinaeVersion

> **vicinaeVersion**: `object`

Defined in: [environment.ts:128](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/environment.ts#L128)

The Vicinae version. Vicinae extensions should rely on this and ignore `raycastVersion`.

#### commit

> **commit**: `string`

#### tag

> **tag**: `string`

## Methods

### canAccess()

> **canAccess**(`api`): `boolean`

Defined in: [environment.ts:119](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/environment.ts#L119)

Returns whether the user has access to the given API.

#### Parameters

##### api

`unknown`

#### Returns

`boolean`

#### Example

```typescript
import { unstableAI, environment } from "@raycast/api";

export default function Command() {
  if (environment.canAccess(unstableAI)) {
    // use unstableAI
  }
}
```
