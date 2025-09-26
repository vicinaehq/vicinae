[**@vicinae/api**](../README.md)

***

[@vicinae/api](../README.md) / Environment

# Interface: Environment

## Properties

### appearance

> **appearance**: `"light"` \| `"dark"`

The appearance used by the Raycast application.

***

### assetsPath

> **assetsPath**: `string`

The absolute path to the assets directory of the extension.

#### Remarks

This directory is used internally to load icons, images, and other bundled assets.

***

### commandMode

> **commandMode**: `"menu-bar"` \| `"no-view"` \| `"view"`

The mode of the launched command, as specified in package.json

***

### commandName

> **commandName**: `string`

The name of the launched command, as specified in package.json

***

### extensionName

> **extensionName**: `string`

The name of the extension, as specified in package.json

***

### isDevelopment

> **isDevelopment**: `boolean`

Indicates whether the command is a development command (vs. an installed command from the Store).

***

### isRaycast

> **isRaycast**: `boolean`

Whether we run an actual Raycast extension in compatibility mode.
This is used internally to provide Raycast-compatible interfaces.

***

### ~~launchContext?~~

> `optional` **launchContext**: [`LaunchContext`](LaunchContext.md)

#### Deprecated

Use the top-level prop `launchContext` instead.

***

### launchType

> **launchType**: [`LaunchType`](../enumerations/LaunchType.md)

The type of launch for the command (user initiated or background).

***

### raycastVersion

> **raycastVersion**: `string`

The version of the main Raycast app

***

### supportPath

> **supportPath**: `string`

The absolute path for the support directory of an extension. Use it to read and write files related to your extension or command.

***

### textSize

> **textSize**: `"medium"` \| `"large"`

The text size used by the Raycast application.

***

### ~~theme~~

> **theme**: `"light"` \| `"dark"`

The theme used by the Raycast application.

#### Deprecated

Use `appearance` instead

***

### vicinaeVersion

> **vicinaeVersion**: `object`

The Vicinae version. Vicinae extensions should rely on this and ignore `raycastVersion`.

#### commit

> **commit**: `string`

#### tag

> **tag**: `string`

## Methods

### canAccess()

> **canAccess**(`api`): `boolean`

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
