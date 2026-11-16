# Native Markdown editor

`vicinae::editor` is a Qt Quick library, exposed as `Vicinae.Editor`. It depends on
Qt Quick and KDE SyntaxHighlighting, without any dependency on the launcher,
notes storage, theme service, or Markdown preview renderer.

`MarkdownEditor` presents a single plain-text `TextEdit` in a scrollable viewport.
`MarkdownEditorDocument` attaches to its native `QTextDocument` and applies the
KDE Markdown grammar, including the grammars for fenced code. Highlighting does
not modify the source or the undo history. The host supplies fonts and colors.
Call `EditorDocument::setPalette(Syntax::Palette)` with the host's semantic colors
when its theme changes. The same palette supplies code colors to the preview
renderer. The palette names syntax roles rather than UI accent colors. Authored
colors are preserved unless they need a contrast correction against the editor
background. Markdown headings, emphasis markers, links, destinations, and inline
code have their own roles, independent of programming-language strings. Embedded code
keeps its own grammar, including when tokens share names with Markdown tokens.

The host owns the document controller and passes it to the component's
`document` property. Call `load(id, markdown)` when switching documents; this
resets selection and undo history. Keep the component alive when showing a
preview to retain its cursor, scroll position, selection, and undo history.
The `textChanged` signal marks content edits; reading `text()` produces a full
snapshot, so hosts should read it when saving rather than after every keystroke.
The attached text item explicitly observes its viewport, so large notes only
generate graphics for the visible text. Qt normally enables this in
`TextEdit::setText`, which direct `QTextDocument` loads and edits bypass.
Caret visibility follows focus and viewport bounds, restoring Qt's off-screen
caret when a click moves the insertion point back into view.
The text item becomes visible once component construction has finished and its
geometry is valid. Otherwise an early accessibility bounds query can read
`implicitWidth`, which makes Qt measure the entire document both unwrapped and
wrapped on every subsequent edit. Accessibility can query the whole window in
response to a sibling's initialization, before the editor's own bindings run.

For contextual edits, `snapshot(start, end)` captures the selected text,
document identity, load generation, and revision. Positions use Qt's UTF-16
offsets. `replace(snapshot, replacement)` makes one undoable edit and rejects
the result if the document has changed since the snapshot. This supports future
rewrite actions without putting AI services or application actions in the editor.
Call these APIs on the GUI thread and commit any input-method composition before
initiating an external action.

The component also provides native clipboard operations, formatting shortcuts,
list continuation, and literal in-note find. Launcher search remains the host's
responsibility and uses Vicinae's fuzzy search.

Run the editor and notes persistence checks from the repository root:

```sh
cmake --build build --target vicinae-notes-tests
QT_QPA_PLATFORM=offscreen build/bin/vicinae-notes-tests
make qmllint
```
