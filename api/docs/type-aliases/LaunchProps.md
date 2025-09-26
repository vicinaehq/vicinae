[**@vicinae/api**](../README.md)

***

[@vicinae/api](../README.md) / LaunchProps

# Type Alias: LaunchProps\<T\>

> **LaunchProps**\<`T`\> = `object`

## Type Parameters

### T

`T` *extends* `object` = \{ `arguments`: [`Arguments`](../interfaces/Arguments.md); `draftValues`: [`Values`](../@vicinae/namespaces/Form/type-aliases/Values.md); `launchContext?`: [`LaunchContext`](../interfaces/LaunchContext.md); \}

## Properties

### arguments

> **arguments**: `T`\[`"arguments"`\]

Use these values to populate the initial state for your command.

***

### draftValues?

> `optional` **draftValues**: `T`\[`"draftValues"`\]

When a user enters the command via a draft, this object will contain the user inputs that were saved as a draft.
Use its values to populate the initial state for your Form.

***

### fallbackText?

> `optional` **fallbackText**: `string`

When the command is launched as a fallback command, this string contains the text of the root search.

***

### launchContext?

> `optional` **launchContext**: `T`\[`"launchContext"`\]

When the command is launched programmatically via `launchCommand`, this object contains the value passed to `context`.

***

### launchType

> **launchType**: [`LaunchType`](../enumerations/LaunchType.md)

The type of launch for the command (user initiated or background).
