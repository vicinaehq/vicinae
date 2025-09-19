[**@vicinae/api**](../../../README.md)

***

[@vicinae/api](../../../README.md) / WindowManagement

# WindowManagement

Access Vicinae's window management features.

## Remarks

Window management features are available to a different degree depending on what environment vicinae runs
in.

## Example

```typescript
import { WindowManagement } from '@vicinae/api';

const windows = await WindowManagement.getWindows();
```

## Type Aliases

- [Window](type-aliases/Window.md)
- [Workspace](type-aliases/Workspace.md)

## Functions

- [getActiveWindow](functions/getActiveWindow.md)
- [getActiveWorkspace](functions/getActiveWorkspace.md)
- [getWindows](functions/getWindows.md)
- [getWindowsOnActiveWorkspace](functions/getWindowsOnActiveWorkspace.md)
- [getWorkspaces](functions/getWorkspaces.md)
- [ping](functions/ping.md)
- [setWindowBounds](functions/setWindowBounds.md)
