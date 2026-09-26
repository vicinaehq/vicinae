# Vendored Kirigami WheelHandler

- Upstream: https://invent.kde.org/frameworks/kirigami (src/wheelhandler.h, src/wheelhandler.cpp)
- Commit: 85ac54062cf9e0b8c1ec9282fc51cca455bd1c1f (2025-09-21)
- License: LGPL-2.0-or-later (see COPYING)

Adapted to be standalone: the Kirigami Units/Settings singletons were replaced
with a constant animation duration and a `smoothScroll` property.
The C++ types live in `vicinae::scrolling`, and QML consumers import
`Vicinae.Scrolling as Scrolling` to distinguish `Scrolling.WheelHandler`
from QtQuick's own WheelHandler.
The touchpad inertia animation is exposed as an `inertiaScroll` property and
re-clamped against the target's live bounds on every animation tick, working
around https://bugs.kde.org/show_bug.cgi?id=508229.
Scroll animations apply incremental movement so viewport corrections from
virtualized layouts remain intact while scrolling, with live bounds for both
wheel and inertia animations.
