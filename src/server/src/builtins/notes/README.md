# Notes window

The toolbar offers Edit, Side by side, and Preview. Side by side keeps the
native editor on the left and a live preview on the right, with a draggable
divider and independent scrolling. Find stays in the editor pane. Switching
views retains the editor document, selection, and undo history.

Live preview refreshes on the next event-loop turn, without waiting for a pause
in typing. Parsing runs off the UI thread, with overlapping requests coalesced.
Completed snapshots keep appearing during continuous typing. Updates preserve
unchanged preview blocks so editing does not reset the reading position or
relayout the whole note.

Notes use a normal, resizable top-level window. The window manager decides
whether it floats or tiles; it is not a dialog or an always-on-top utility.

The window title always starts with `Vicinae Notes — `, followed by the note's
title. This prefix stays unchanged when switching notes or languages. Match
`^Vicinae Notes — ` in window rules to distinguish notes from the launcher,
settings, and chat windows.

On X11, `WM_WINDOW_ROLE` is also `vicinae-notes`. The Wayland app ID and X11
application class remain Vicinae's existing application identity, so desktop
integration and application grouping are preserved. On Wayland, combine that
identity with the title prefix when configuring a floating rule. The QML object
name is `vicinae-notes` for diagnostics; object names are not compositor IDs.
