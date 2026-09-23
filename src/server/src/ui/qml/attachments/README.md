# Attachments

`AttachmentModel` prepares image and UTF-8 text files in C++. It exposes lightweight
`items` for presentation; file contents stay in immutable C++ storage.
It does not know about AI providers or conversation storage.

- `AttachmentCard` displays one item, optionally with a remove button. It has a
  fixed height and uses `ViciImage` for image thumbnails and native document icons.
  The model supplies file-icon sources using the original file paths, preserving
  the platform's file-type icons in both the composer and sent messages.
- `AttachmentList` wraps cards to the available width. The composer and sent
  messages use the same component.
- `AttachmentMenu` provides native context-menu actions to copy the original file,
  copy text/image contents, preview images, and remove draft attachments. Clicking
  anywhere on an image card requests a preview, except its remove button.
- `AttachmentPreview` opens the existing `ImageViewer`, created only while needed.
  Place it outside virtualized delegates and route `previewRequested` from the
  list/composer to its `show(content)` method. Quick AI shares one preview between
  its composer and document, so recycling a message card does not close it.
  It supplies the viewer's `contextMenu` with the same attachment copy actions,
  hiding Preview Image while the image is already open.
- `AttachmentDropArea` accepts local file URLs into a model.
- `AttachmentPasteHandler` attaches to a text input and handles image/file paste;
  ordinary text paste stays with the input.
- `FilePickerDialog` shares the platform chooser and Qt fallback used by both
  the attachment composer and `FormFilePicker`.

A host can own an `AttachmentModel` and expose it as a constant property. Bind
`AttachmentList.attachments` to `model.items`, route removal to `model.remove(id)`,
and bind a paste handler to the editor. `ready` is false while files are loading
or an item has an error. Errors stay on individual cards so the user can remove
an unsupported file without losing the rest of the draft.

Call `take()` only after accepting the submission. Snapshot `items` before taking
attachments if the submitted message needs previews. `take()` moves shared
`AttachmentContent` handles out and clears the draft; removing an item during preparation prevents
its asynchronous result from reappearing.

The `content` value in each item is an `AttachmentContent` handle to the same
immutable prepared data. Copies of the metadata share that storage, so sent
messages can still copy and preview their attachments after the draft clears.
Storage is released when the last handle is dropped. Copy Text and Copy Image
use the prepared snapshot; Copy File uses the original file path and reports
when that file is no longer available. No temporary files are created for paste.

In Quick AI, the host turns prepared images into `AI::ImagePart` and text files
into labeled `AI::TextPart` values. The conversation model retains preview metadata
alongside the query. Attachment cards are controls, not document selection
surfaces: copying a text selection or the whole conversation includes message
text, without attachment filenames or contents. Attachment copying uses the
card's own context menu.

The current limits are eight files per message, 50 MiB per source file, and
128 KiB of UTF-8 text in total. Images are oriented using their metadata, scaled
to fit 2048 × 2048, and encoded as PNG off the UI thread. Thumbnails fit 256 × 256;
cards load only these small previews; opening the image viewer requests the full
prepared image. Animated images use their first
frame. PDFs, office documents, and non-UTF-8 text are not converted.

The Quick AI host checks vision support against both pending attachments and
images already present in the current conversation. Switching to a text-only
model keeps the draft and disables sending until a compatible model is selected.
