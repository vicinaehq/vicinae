# Launcher content geometry

`LauncherPage` wraps every command view. Its loader occupies the area below the
overlay header, so ordinary item geometry, layouts, previews, and empty states
need no search-bar padding. The platform appearance supplies the header inset;
it is zero when the window already reserves space for the search bar.

Shared scrolling components use `ScrollViewport`. It extends the scrolling
surface behind the header while keeping the component's layout inside the page's
content area. It owns scroll margins, initial positioning, selection visibility,
scrollbar clearance, and scroll bounds. Lists and grids retain their native Qt
virtualization. Side-by-side panes remain independent; nested scrollers and
scrollers below fixed controls keep their local bounds.

New feature views should use the existing list, grid, form, markdown, or preview
components. A new scrolling primitive should wrap its Flickable in
`ScrollViewport` and set `flickable`; it should not measure the launcher header.
Fixed content needs no wrapper or inset helper.

Covered pages retain their inset. Updates are deferred until navigation's header
and page changes have settled, preventing temporary geometry changes from
altering the saved scroll position. Page activation forwards `restoreFocus()`
when the loaded view implements it.
