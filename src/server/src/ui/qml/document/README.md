# Composable documents

A document coordinates selection across ordinary QML components. Set
`DocumentScope.document` once on their common container; nested components,
Loaders, and delegates inherit it without forwarding properties.

```qml
DocumentController {
    id: doc
    container: contents
    flickable: viewport
}

Flickable {
    id: viewport
    anchors.fill: parent
    contentWidth: width
    contentHeight: contents.height

    Column {
        id: contents
        width: viewport.width
        spacing: 12
        DocumentScope.document: doc

        DocumentText {
            width: parent.width
            text: qsTr("A document can contain any QML component.")
        }
        MarkdownInline {
            width: parent.width
            height: implicitHeight
            markdown: "## Details\n\nMarkdown is one document renderer."
        }
        AttachmentCard {
            width: parent.width
            filename: "report.pdf"
        }
    }
}
```

`DocumentText` is a themed, read-only TextEdit with document selection. It keeps
TextEdit's normal font, wrapping, rich-text, sizing, and layout properties. Use
`DocumentTextSelection {}` inside an existing TextEdit to adapt it without
replacing the component or changing its styling.

The document imposes no layout. It owns drag and shift-click selection, word
selection, Copy/Select All shortcuts, and scrolling during a drag. Reading order
follows visual positions, top to bottom and then left to right. Interactive
children such as buttons keep their own input handling.

## Reusable widgets

A component declares its own selection behavior without depending on a caller's
IDs. The adapter's target defaults to its containing Item, and its document comes
from that target's inherited scope.

```qml
// AttachmentCard.qml
Rectangle {
    id: card
    property string filename
    implicitHeight: 48
    color: selection.hasSelection ? Theme.textSelectionBg : Theme.secondaryBackground

    DocumentText {
        anchors.centerIn: parent
        text: card.filename
        // The enclosing card represents one selection unit.
        DocumentScope.document: null
    }

    DocumentSelection {
        id: selection
        selectedText: card.filename
    }
}
```

`DocumentSelection` defaults to one atomic selection unit. A pointer on either
half of the target selects the boundary before or after it; double-click selects
the entire unit. It is also suitable for images, attachments, and other widgets
whose copy representation differs from their visible content.

A more complex widget can define its own units and hit testing. For example,
place this adapter inside a card whose rows have a fixed height:

```qml
DocumentSelection {
    id: selection
    length: card.rows.length
    hitTest: (x, y) => Math.round(y / card.rowHeight)
    positionRectangle: position => Qt.rect(0, position * card.rowHeight, card.width, card.rowHeight)
    wordRange: position => ({
        start: Math.min(position, length - 1),
        end: Math.min(position + 1, length)
    })
    selectedText: card.rows.slice(selectionStart, selectionEnd).join("\n")
}
```

Positions range from `0` to `length`. The selected range is half-open:
`[selectionStart, selectionEnd)`. The widget draws its highlight from those
properties or `hasSelection`. `hitTest` receives target-local coordinates;
`wordRange` supplies double-click boundaries. Returned positions are clamped.
An optional `linkAt(x, y)` callback returns a link or an empty string.
`positionRectangle(position)` supplies the target-local rectangle for a logical
position, allowing the viewport to keep that content in place during reflow.
Atomic widgets default to their bounds; text adapters use native cursor geometry.

`selectedText` supplies the participant's plain-text copy representation. It can
be bound to its selected range or remain constant for an atomic widget. The
document joins nonempty selected representations with blank lines. C++ widgets
can subclass `DocumentSelection` and override `applySelection`, `length`,
`positionAt`, `rectangleAt`, `wordAt`, `linkAtPosition`, and `selectedText`; the base class
maintains the normalized range before invoking `applySelection`.

## Scope and lifetime

Scopes follow the visual item tree, including when a reusable subtree changes
parents. Inheritance uses Qt's
[attached-property propagation](https://doc.qt.io/qt-6/qquickattachedpropertypropagator.html).
Intermediate scopes are shared by participating descendants and keep reparenting
correct across ordinary Items.

- Set `DocumentScope.document` on a nested container to start a separate document.
- Set it to `null` to exclude a subtree. Assign `undefined` to resume inheritance.
- An individual adapter can override `document` or `target` explicitly. Reset its
  `document` to `undefined` to inherit again, or use `enabled: false` to detach it.
- Hidden or destroyed targets detach automatically. Text adapters restore their
  original mouse and Tab-focus settings when detached.

For a Repeater whose rows are addressed by index, use `IndexModel { count: ... }`
when its length changes incrementally. It inserts and removes only the changed
indexes, preserving existing delegates. Bind each delegate's content through its
index into the source data. Table rendering uses this to keep existing cells and
selection stable while later cells and rows arrive.

Components require no registration or destruction handlers. A custom selectable
surface should let pointer events reach the document's Flickable where document
selection gestures are desired; interactive children can continue handling
clicks themselves.

`MarkdownInline` uses the surrounding scope. The standalone `MarkdownView` owns
its scrolling surface and exposes its controller as `document`. Markdown's model
handles parsing and code-block actions; the document knows nothing about Markdown.
Link clicks emit `DocumentController.linkActivated(link)` for the host to handle.
`DocumentSelectionMenu` provides the shared native Copy and Select All menu.
Attach it with `ContextMenu.menu` so embedded controls can provide their own menus.

## Virtual documents

Use `DocumentView` for large content. It is a native Flickable with virtualized delegates, a document
controller, and one viewport of cache. Delegates identify their logical model row
once; their reusable descendants inherit it with the document scope.

```qml
DocumentView {
    id: view
    anchors.fill: parent
    documentModel: host.documentModel
    delegate: AttachmentCard {
        required property int index
        required property string filename
        DocumentScope.row: index
        width: view.width
    }
}
```

The host supplies a `DocumentModel`, a QAbstractListModel with ordinary display
roles and `documentParts(row)`. Each part supplies plain text and whether it is an
atomic selection unit. The model owns this data independently of QML delegates.
For specialized units, override `partLength` and `textForSelection` as well.

A row can contain several selectable widgets. Give each adapter its corresponding
zero-based `part`, or set `DocumentText.selectionPart`. Keep parts in logical
reading order; a table, for example, numbers its cells by row then column.
The default is part zero. Model parts must match the rendered content, including
hidden or conditional content. Use normal Qt model notifications for changes.

Selection endpoints use persistent model indexes, part numbers, and positions.
Unmounting a delegate preserves selection; newly mounted widgets immediately
restore their highlight. Copy and Select All include off-screen content without
creating delegates. Removing a selected endpoint or resetting the model clears
selection. Appending text does not extend an existing selection automatically.

`MarkdownView` and Quick Chat use this path. Quick Chat exposes each response
block separately, so a single long reply is virtualized too. Bubble decoration
is composed around these rows. `MarkdownBlock` is shared by both views and by
`MarkdownInline`, which remains an eager renderer for small embedded fragments.

`DocumentLayout` caches row heights and measures unvisited rows in small idle
batches. Reading anchors preserve logical text positions during image loading,
font changes, and resizing, with row offsets as a fallback. Nested virtual layouts
preserve their own row anchors before recycling cells during reflow. Rows keep
their ordinary QML layout. Tables reuse an embedded `DocumentLayout` with a
`rowHeight` model role, so only nearby table rows create cell widgets.
The model and full-document copy scale with content size.

`DocumentText` installs `DocumentTextImages` for rich-text image layout. Image
dimensions survive delegate recycling; decoded images use a bounded 64 MiB cache
shared within the QML engine, with a separate bounded cache for dimensions.
Background measurement uses known dimensions without fetching offscreen images.
Table measurement uses the same image sizing. A custom `TextEdit` can use the
same adapter by binding `document` to its `textDocument` and `measuring` to its
`DocumentScope.measuring`. Explicit image dimensions reserve space before the
first load; otherwise the natural dimensions become available when it loads.

## Streaming and performance

For the eager, component-based path, settle layout after content changes and
call `document.restoreSelection()`. `MarkdownInline` already does this.
Model-backed documents restore from model notifications and participant mounts.

Set `DocumentView.followEnd` while a streaming view should stay at the bottom.
It updates the scroll position synchronously with layout, including viewport
margins, so a newly wrapped line and its surrounding decoration move together.
The end position uses the layout's accumulated row heights.
Disable it when the reader scrolls or selects text. `scrollToEnd()` performs a
one-time jump without changing that policy.

Selection updates skip unchanged ranges and native TextEdit writes. Geometry is
checked only for mounted participants, and their reading order is sorted only
when positions or membership change. Scopes resolve on composition changes.

## Exercising the document

Use the real application for selection, scrolling, layout, and focus checks.
Keep realistic long replies, tables, and image-heavy content in those scenarios.
Check selection across bubbles, copy after scrolling away, resizing inside tables,
image loading and revisiting, fast direction changes, and typing after selection.
Exercise streaming and the actual jump button in Quick AI itself. These are the
scenarios for future end-to-end automation; today they are exercised through the UI.

The focused `vicinae-document-tests` executable retains only internal invariants
that screen-level checks cannot reliably inspect: Qt model notifications and
persistent indexes, grapheme boundaries on every published stream update, and
image dimensions after decoded-cache eviction. It creates no QML views or mock
renderers. The Unicode check waits for completion without asserting frame timing.

```sh
cmake --build build --target vicinae-document-tests
QT_QPA_PLATFORM=offscreen build/bin/vicinae-document-tests
```
