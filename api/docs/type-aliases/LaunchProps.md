[**@vicinae/api**](../README.md)

***

[@vicinae/api](../README.md) / LaunchProps

# Type Alias: LaunchProps\<T\>

> **LaunchProps**\<`T`\> = `object`

Defined in: [environment.ts:11](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/environment.ts#L11)

## Type Parameters

### T

`T` *extends* `object` = \{ `arguments`: [`Arguments`](../interfaces/Arguments.md); `draftValues`: [`Values`](../@vicinae/namespaces/Form/type-aliases/Values.md); `launchContext?`: [`LaunchContext`](../interfaces/LaunchContext.md); \}

## Properties

### arguments

> **arguments**: `T`\[`"arguments"`\]

Defined in: [environment.ts:29](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/environment.ts#L29)

Use these values to populate the initial state for your command.

***

### draftValues?

> `optional` **draftValues**: `T`\[`"draftValues"`\]

Defined in: [environment.ts:34](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/environment.ts#L34)

When a user enters the command via a draft, this object will contain the user inputs that were saved as a draft.
Use its values to populate the initial state for your Form.

***

### fallbackText?

> `optional` **fallbackText**: `string`

Defined in: [environment.ts:42](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/environment.ts#L42)

When the command is launched as a fallback command, this string contains the text of the root search.

***

### launchContext?

> `optional` **launchContext**: `T`\[`"launchContext"`\]

Defined in: [environment.ts:38](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/environment.ts#L38)

When the command is launched programmatically via `launchCommand`, this object contains the value passed to `context`.

***

### launchType

> **launchType**: [`LaunchType`](../enumerations/LaunchType.md)

Defined in: [environment.ts:25](https://github.com/vicinaehq/vicinae/blob/c742d5fc509336339909dd669955b863f086bf4e/api/src/api/environment.ts#L25)

The type of launch for the command (user initiated or background).
