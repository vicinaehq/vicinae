# StackView / C++ View Host Lifecycle Issue

## Problem

When a QML view is popped from the StackView, the C++ view host (`QmlBridgeViewBase`) can be destroyed before the QML item that references it. This causes QML bindings like `root.host.someProperty` to re-evaluate with a null `host`, producing `TypeError: Cannot read property 'xxx' of null` warnings.

## Root Cause

Two different destruction mechanisms with different timing guarantees:

1. **QML items** created by StackView from URLs get `JavaScriptOwnership`. When popped, StackView detaches them and leaves them to the **non-deterministic** JS garbage collector.

2. **C++ view hosts** are destroyed by `~ViewState()` via `deleteLater()`, which fires **deterministically** on the next event loop iteration.

The C++ host is almost always destroyed first. During its `QObject::~QObject()`, the `destroyed()` signal fires. QML's property tracking detects this, nullifies the reference, and re-evaluates all bindings that depend on it -- while the QML item is still alive waiting for GC.

### Destruction sequence (before fix)

```
pop(Immediate) called
  -> StackView detaches QML item, queues for GC
  -> ~ViewState() -> sender->deleteLater()

Next event loop:
  -> deleteLater fires -> C++ host destroyed
  -> QML bindings re-evaluate with null host  <-- TypeErrors here

Eventually:
  -> GC runs -> QML item destroyed (too late)
```

## Current Fix

In `NavigationController::ViewState::~ViewState()` (navigation-controller.cpp), the deletion is double-deferred:

```cpp
auto *s = sender;
QTimer::singleShot(0, s, [s]() { s->deleteLater(); });
```

This pushes the C++ host destruction back by one extra event loop cycle, giving the QML GC time to collect the detached item first.

### Destruction sequence (after fix)

```
pop(Immediate) called
  -> StackView detaches QML item, queues for GC
  -> ~ViewState() -> QTimer::singleShot(0, ...) queued

Next event loop:
  -> GC runs -> QML item destroyed (no more bindings)
  -> singleShot fires -> deleteLater() queued

Next event loop:
  -> deleteLater fires -> C++ host destroyed (QML item is already gone)
```

## Why Not Other Approaches

| Approach | Why it doesn't work |
|---|---|
| `item.destroy()` after `pop()` | Crashes -- StackView still holds internal references to the item after `pop()` returns |
| `gc()` after `pop()` | `gc()` is a hint, not a guarantee; unreliable |
| Null guards on every binding | Works but tedious, error-prone on new views, and masks the real issue |
| `required property` | Only guarantees the property is set at creation time; doesn't prevent the referenced QObject from being destroyed later |

## Caveats

The double-deferred `deleteLater` is a timing-based fix. It works in practice because the QML GC runs during event loop processing and one extra cycle is sufficient. But it is **not a hard guarantee** -- if the GC were delayed for some reason, the issue could resurface.

If this becomes unreliable, potential alternatives:
- Move view host ownership out of `ViewState` entirely and into `QmlLauncherWindow`, which can explicitly control destruction order relative to the StackView
- Pre-create QML items with `Component.createObject(parent, properties)` and push the item (not URL) to StackView, giving explicit C++ ownership over the QML item lifetime
- Add null guards to all view QML files as a fallback (ClipboardHistoryView already has them)

## Files Involved

- `navigation-controller.cpp` -- `~ViewState()` destructor (the fix)
- `qml/qml/LauncherWindow.qml` -- StackView pop/clear handlers
- `qml/qml-launcher-window.cpp` -- C++ bridge that emits push/pop/clear signals
- `navigation-controller.hpp` -- `ViewState` struct definition
