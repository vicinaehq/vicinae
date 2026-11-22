# Notes window

Notes are ordinary UTF-8 `.md` files. The filename is the title; renaming a note
renames its file, with a numeric suffix when that name is already taken. There is
no required frontmatter or proprietary format. Notes are not encrypted by Vicinae.

Pasting a screenshot saves an image beside the note in `<title>.assets/` and
inserts a standard Markdown image with a relative path. Copied files are copied
into the same folder and inserted as images or file links. Existing PNG, JPEG,
GIF, and WebP clipboard data is preserved; native clipboard images are encoded
as PNG. Text and web URLs retain normal text paste behavior. File copying and
image encoding run on the notes worker thread.

Dropping images or local files into the editor uses the same attachment flow,
inserting at the pointer with a visible caret and automatic scrolling near the
edges. Text and web URLs can also be dropped. Dragging a rendered image from the
preview or enlarged image viewer copies it into another note or app. Local images offer both the original
file and native image data, so file managers retain the original file format.
These drags never move or delete the source image.

Attachment folders are created when adding the first attachment. Their names remain stable when
a note is renamed, and later attachments reuse the folder referenced by the note.
Copy the Markdown file and its referenced attachment folder together to move a
note. **Save a Copy** carries referenced attachments to the destination directory
without overwriting different existing files. Attachment Markdown files are not
indexed as notes. Removing a link, undoing a paste, or deleting a note leaves its
attachments on disk, preserving redo and links from other notes.

Existing frontmatter is kept as ordinary source text; Vicinae does not add,
interpret, or rewrite YAML properties. The filename remains the note's title.

**Settings → Notes → Preferences** uses the extension preferences system.
**Default directory** chooses where new notes are saved; **Additional directories**
adds other folders to search.
Search includes Markdown files in all configured directories and their subdirectories.
The default directory is always included in search. Changing it does not move existing
notes; add the previous location to Additional directories to keep searching it.
Removing a directory from preferences does not delete its files. A missing
or read-only default produces a save error instead of silently using another location.
Read-only files (including symlinked files) can be viewed, searched, and copied.

By default, notes live in `notes/` inside Vicinae's data directory. Files are written
atomically. External changes refresh unedited notes automatically; if a file changes
while you are editing, saving stops and offers **Save a Copy** or **Reload**.

Existing `notes/notes.db` content is imported automatically on first use into the
default directory. The database is retained as a backup. A hidden
`notes/.database-import.json` record makes migration resumable and prevents deleted
imports from reappearing. Previously encrypted notes databases are decrypted using
the existing keychain key before import.

Directories can also be configured declaratively, including through Home Manager:

```nix
programs.vicinae.settings.providers.notes.preferences = {
  defaultDirectory = "/home/me/Notes";
  directories = [ "/home/me/reference-notes" ];
};
```

Use an existing writable directory for `defaultDirectory`. Read-only Nix-managed
files can live in the additional directories. Without an explicit default, Vicinae
uses its built-in notes directory.
`~` is supported in paths. No app-specific files are needed alongside normal notes.

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
