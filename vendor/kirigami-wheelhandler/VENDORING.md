# Vendored Kirigami WheelHandler

- Upstream: https://invent.kde.org/frameworks/kirigami (src/wheelhandler.h, src/wheelhandler.cpp)
- Commit: 85ac54062cf9e0b8c1ec9282fc51cca455bd1c1f (2025-09-21)
- License: LGPL-2.0-or-later (see COPYING)

Adapted to be standalone: the Kirigami Units/Settings singletons were replaced
with a constant animation duration and a `smoothScroll` property, and the QML
type names were prefixed to avoid clashing with QtQuick's own WheelHandler.
The touchpad inertia animation is exposed as an `inertiaScroll` property and
re-clamped against the target's live bounds on every animation tick, working
around https://bugs.kde.org/show_bug.cgi?id=508229.
