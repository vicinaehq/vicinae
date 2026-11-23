#include <KSyntaxHighlighting/Repository>
#include <QDir>
#include <QFile>
#include <QImage>
#include <QMimeData>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTest>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextLayout>
#include <QUrl>
#include "builtins/notes/notes-tools.hpp"
#include "editor-document.hpp"
#include "services/notes/notes-front-matter.hpp"
#include "services/notes/notes-store.hpp"

class NotesInvariantsTest : public QObject {
  Q_OBJECT

private slots:
  void pastedImagesRemainPortableAfterRenamingAndReopening() {
    QTemporaryDir directory;
    QImage image(30, 20, QImage::Format_ARGB32);
    image.fill(QColor(40, 90, 180, 120));
    QMimeData mime;
    mime.setImageData(image);
    QVERIFY(Notes::canImportAttachments(&mime));
    const auto attachments = Notes::attachmentsFromMimeData(&mime);
    QCOMPARE(attachments.size(), std::size_t(1));
    const std::string frontmatter = "---\ntags: [work]\ncustom: untouched\n---\n\n";
    QString originalFolder;
    {
      Notes::Store store(directory.path().toStdString());
      auto imported = store.attach({"id", "Meeting #计划", frontmatter}, true, attachments).result();
      QVERIFY(imported);
      QVERIFY(imported->markdown);
      QCOMPARE(imported->note.markdown, frontmatter);
      const auto markdown = QString::fromStdString(*imported->markdown);
      const auto destination = markdown.mid(markdown.indexOf(u"(<") + 2).chopped(2);
      const auto relative = QUrl(destination).path();
      QVERIFY(relative.startsWith(u"Meeting #计划.assets/"));
      QCOMPARE(QImage(directory.filePath(relative)), image);
      originalFolder = QFileInfo(relative).path();
      auto note = imported->note;
      note.markdown += *imported->markdown;
      auto saved = store.save(note, false).result();
      QVERIFY(saved);
      saved->title = "Renamed";
      saved = store.save(*saved, false).result();
      QVERIFY(saved);
      QCOMPARE(saved->markdown, note.markdown);
      QVERIFY(QFileInfo::exists(directory.filePath(relative)));
    }
    {
      Notes::Store store(directory.path().toStdString());
      const auto notes = store.list().result();
      QVERIFY(notes);
      QCOMPARE(notes->size(), std::size_t(1));
      const auto imported = store.attach(notes->front(), false, attachments).result();
      QVERIFY(imported && imported->markdown);
      const auto markdown = QString::fromStdString(*imported->markdown);
      const auto destination = markdown.mid(markdown.indexOf(u"(<") + 2).chopped(2);
      const auto relative = QUrl(destination).path();
      QCOMPARE(QFileInfo(relative).path(), originalFolder);
      QCOMPARE(QDir(directory.filePath(originalFolder)).entryList(QDir::Files).size(), 2);
      QVERIFY(store.remove(notes->front().id).result());
      QVERIFY(QFileInfo::exists(directory.filePath(relative)));
    }
  }

  void copiedFilesAndSaveCopyPreserveAttachmentContents() {
    QTemporaryDir directory;
    QTemporaryDir destination;
    const auto source = directory.filePath("reference [1] #%.md");
    QFile file(source);
    QVERIFY(file.open(QIODevice::WriteOnly));
    const QByteArray contents("# A file attachment\n\nOriginal bytes\n");
    QCOMPARE(file.write(contents), contents.size());
    file.close();
    QMimeData mime;
    mime.setUrls({QUrl::fromLocalFile(source)});
    Notes::Store store(directory.filePath("notes").toStdString());
    auto result = store.attach({"id", "Files", ""}, true, Notes::attachmentsFromMimeData(&mime)).result();
    QVERIFY(result && result->markdown);
    QVERIFY(result->markdown->starts_with("[reference \\[1\\]"));
    auto note = result->note;
    note.markdown = *result->markdown;
    auto saved = store.save(note, false).result();
    QVERIFY(saved);
    QCOMPARE(store.list().result()->size(), std::size_t(1));
    store.configure({.defaultDirectory = destination.path().toStdString()});
    auto copy = *saved;
    copy.id = "copy";
    auto copied = store.save(copy, true).result();
    QVERIFY(copied);
    QCOMPARE(copied->markdown, saved->markdown);
    QFile copiedFile(destination.filePath("Files.assets/reference [1] #%.md"));
    QVERIFY(copiedFile.open(QIODevice::ReadOnly));
    QCOMPARE(copiedFile.readAll(), contents);
    copiedFile.close();
    QVERIFY(copiedFile.open(QIODevice::WriteOnly));
    copiedFile.write("Unrelated contents");
    copiedFile.close();
    copy.id = "collision";
    QVERIFY(!store.save(copy, true).result());
    QVERIFY(!QFileInfo::exists(destination.filePath("Files (2).md")));
    QVERIFY(copiedFile.open(QIODevice::ReadOnly));
    QCOMPARE(copiedFile.readAll(), QByteArray("Unrelated contents"));
  }

  void failedAttachmentImportKeepsTheNoteUsableAndRollsBackFiles() {
    QTemporaryDir directory;
    Notes::Store store(directory.path().toStdString());
    QImage image(10, 10, QImage::Format_RGB32);
    image.fill(Qt::blue);
    const auto result =
        store.attach({"id", "Failure", "Keep this text"}, true, {{"good.png", image}, {"bad.png", QImage()}})
            .result();
    QVERIFY(result);
    QVERIFY(!result->markdown);
    QCOMPARE(result->note.markdown, std::string("Keep this text"));
    QVERIFY(QDir(directory.filePath("Failure.assets")).entryList(QDir::Files).isEmpty());
    const auto retry = store.attach(result->note, false, {{"good.png", image}}).result();
    QVERIFY(retry && retry->markdown);
    QMimeData text;
    text.setText("Ordinary text");
    QVERIFY(!Notes::canImportAttachments(&text));
    QVERIFY(Notes::attachmentsFromMimeData(&text).empty());
    text.setUrls({QUrl("https://example.com/image.png")});
    QVERIFY(!Notes::canImportAttachments(&text));
    QMimeData cells;
    cells.setText("1\t2");
    cells.setImageData(image);
    QVERIFY(!Notes::canImportAttachments(&cells));
  }

  void saveCopySkipsMissingAttachments() {
    QTemporaryDir directory;
    QTemporaryDir destination;
    Notes::Store store(directory.path().toStdString());
    const auto saved = store.save({"id", "Note", "[gone](Note.assets/gone.png)"}, true).result();
    QVERIFY(saved);
    store.configure({.defaultDirectory = destination.path().toStdString()});
    auto copy = *saved;
    copy.id = "copy";
    const auto copied = store.save(copy, true).result();
    QVERIFY(copied);
    QCOMPARE(copied->markdown, saved->markdown);
    QVERIFY(!QFileInfo::exists(destination.filePath("Note.assets")));
  }

  void highlightingPreservesSourceAndUndo() {
    QTextDocument text;
    Vicinae::Editor::EditorDocument editor(m_repository);
    editor.attach(&text);
    const QString source =
        QStringLiteral("# Heading\n\n**bold** and *italic*\n\n```cpp\nint value = 42;\n```\n- [x] Done\n");
    editor.load("note", source);
    QTRY_VERIFY(!text.begin().layout()->formats().isEmpty());
    QCOMPARE(editor.text(), source);
    QVERIFY(!text.isUndoAvailable());
    QTextCursor cursor(&text);
    cursor.movePosition(QTextCursor::End);
    cursor.insertText(QStringLiteral("你好 👋"));
    const auto revision = editor.revision();
    Vicinae::Syntax::Palette palette;
    palette.foreground = Qt::black;
    palette.background = Qt::white;
    editor.setPalette(palette);
    QCoreApplication::processEvents();
    QCOMPARE(editor.revision(), revision);
    QCOMPARE(editor.text(), source + QStringLiteral("你好 👋"));
    text.undo();
    QCOMPARE(editor.text(), source);
    text.redo();
    QCOMPARE(editor.text(), source + QStringLiteral("你好 👋"));
  }

  void replacementsAreUndoableAndRejectStaleSelections() {
    QTextDocument text;
    Vicinae::Editor::EditorDocument editor(m_repository);
    editor.attach(&text);
    editor.load("note-a", QStringLiteral("👋 hello\nworld"));
    const auto selected = editor.snapshot(3, 8);
    QCOMPARE(selected.value("text").toString(), "hello");
    QVERIFY(editor.replace(selected, "bonjour"));
    QCOMPARE(editor.text(), QStringLiteral("👋 bonjour\nworld"));
    QVERIFY(!editor.replace(selected, "stale"));
    text.undo();
    QCOMPARE(editor.text(), QStringLiteral("👋 hello\nworld"));
    const auto previous = editor.snapshot(3, 8);
    editor.load("note-b", editor.text());
    QVERIFY(!editor.replace(previous, "wrong note"));
    const auto beforeReload = editor.snapshot(3, 8);
    editor.load("note-b", editor.text());
    QVERIFY(!editor.replace(beforeReload, "stale generation"));
    QVERIFY(editor.snapshot(-1, 99).isEmpty());
    QVERIFY(!editor.replace({}, "invalid"));
  }

  void textKeepsNonBreakingSpaces() {
    QTextDocument text;
    Vicinae::Editor::EditorDocument editor(m_repository);
    editor.attach(&text);
    const auto source = QStringLiteral("mot\u00a0: 10\u00a0km\nnext");
    editor.load("nbsp", source);
    QCOMPARE(editor.text(), source);
  }

  void listContinuationHasOneUndoStep() {
    QTextDocument text;
    Vicinae::Editor::EditorDocument editor(m_repository);
    editor.attach(&text);
    editor.load("note", "- [x] Complete");
    const auto position = editor.continueLine(text.characterCount() - 1);
    QCOMPARE(editor.text(), "- [x] Complete\n- [ ] ");
    QCOMPARE(position, text.characterCount() - 1);
    text.undo();
    QCOMPARE(editor.text(), "- [x] Complete");
    text.redo();
    editor.continueLine(text.characterCount() - 1);
    QCOMPARE(editor.text(), "- [x] Complete\n");
    editor.load("note", "9. Item");
    editor.continueLine(text.characterCount() - 1);
    QCOMPARE(editor.text(), "9. Item\n10. ");
  }

  void formattingTogglesWithoutLosingTheSelection() {
    QTextDocument text;
    Vicinae::Editor::EditorDocument editor(m_repository);
    editor.attach(&text);
    editor.load("note", "hello");
    QSignalSpy selections(&editor, &Vicinae::Editor::EditorDocument::selectionRequested);
    editor.wrap(0, 5, "**");
    QCOMPARE(editor.text(), "**hello**");
    QCOMPARE(selections.last().at(0).toInt(), 2);
    QCOMPARE(selections.last().at(1).toInt(), 7);
    editor.wrap(2, 7, "**");
    QCOMPARE(editor.text(), "hello");
    text.undo();
    QCOMPARE(editor.text(), "**hello**");
  }

  void selectionFormatting_data() {
    using Editor = Vicinae::Editor::EditorDocument;
    QTest::addColumn<QString>("source");
    QTest::addColumn<int>("start");
    QTest::addColumn<int>("end");
    QTest::addColumn<int>("format");
    QTest::addColumn<QString>("expected");
    QTest::addColumn<QString>("selected");
    QTest::newRow("bold-unicode") << QString("hello 👋") << 6 << 8 << int(Editor::Bold)
                                  << QString("hello **👋**") << QString("👋");
    QTest::newRow("italic") << QString("hello") << 0 << 5 << int(Editor::Italic) << QString("*hello*")
                            << QString("hello");
    QTest::newRow("italic-inside-bold") << QString("**hello**") << 2 << 7 << int(Editor::Italic)
                                        << QString("***hello***") << QString("hello");
    QTest::newRow("bold-inside-italic")
        << QString("*hello*") << 1 << 6 << int(Editor::Bold) << QString("***hello***") << QString("hello");
    QTest::newRow("italic-around-bold") << QString("**hello**") << 0 << 9 << int(Editor::Italic)
                                        << QString("***hello***") << QString("**hello**");
    QTest::newRow("strike") << QString("hello") << 0 << 5 << int(Editor::StrikeThrough)
                            << QString("~~hello~~") << QString("hello");
    QTest::newRow("bold-line-selection") << QString("hello\nnext line") << 0 << 6 << int(Editor::Bold)
                                         << QString("**hello**\nnext line") << QString("hello");
    QTest::newRow("italic-line-selection") << QString("hello\nnext line") << 0 << 6 << int(Editor::Italic)
                                           << QString("*hello*\nnext line") << QString("hello");
    QTest::newRow("strike-line-selection")
        << QString("hello\nnext line") << 0 << 6 << int(Editor::StrikeThrough)
        << QString("~~hello~~\nnext line") << QString("hello");
    QTest::newRow("inline-line-selection") << QString("hello\nnext line") << 0 << 6 << int(Editor::InlineCode)
                                           << QString("`hello`\nnext line") << QString("hello");
    QTest::newRow("bold-boundary-whitespace")
        << QString("  hello  \n\nnext line") << 0 << 11 << int(Editor::Bold)
        << QString("  **hello**  \n\nnext line") << QString("hello");
    QTest::newRow("bold-unicode-line-selection") << QString("👋\nnext line") << 0 << 3 << int(Editor::Bold)
                                                 << QString("**👋**\nnext line") << QString("👋");
    QTest::newRow("inline") << QString("hello") << 0 << 5 << int(Editor::InlineCode) << QString("`hello`")
                            << QString("hello");
    QTest::newRow("embedded-ticks") << QString("a `tick` here") << 0 << 13 << int(Editor::InlineCode)
                                    << QString("``a `tick` here``") << QString("a `tick` here");
    QTest::newRow("edge-ticks") << QString("`tick") << 0 << 5 << int(Editor::InlineCode)
                                << QString("`` `tick ``") << QString("`tick");
    QTest::newRow("selected-code-span")
        << QString("`hello`") << 0 << 7 << int(Editor::InlineCode) << QString("hello") << QString("hello");
    QTest::newRow("heading-partial") << QString("hello world\nnext") << 0 << 5 << int(Editor::Heading2)
                                     << QString("## hello world\nnext") << QString("hello");
    QTest::newRow("heading-six") << QString("title") << 0 << 5 << int(Editor::Heading6)
                                 << QString("###### title") << QString("title");
    QTest::newRow("bullets-exclusive-end")
        << QString("one\ntwo\nuntouched") << 0 << 8 << int(Editor::BulletList)
        << QString("- one\n- two\nuntouched") << QString("one\n- two\n");
    QTest::newRow("numbered") << QString("one\ntwo") << 0 << 7 << int(Editor::NumberedList)
                              << QString("1. one\n2. two") << QString("one\n2. two");
    QTest::newRow("tasks") << QString("one\ntwo") << 0 << 7 << int(Editor::TaskList)
                           << QString("- [ ] one\n- [ ] two") << QString("one\n- [ ] two");
    QTest::newRow("indented-list") << QString("  one\n  two") << 2 << 11 << int(Editor::BulletList)
                                   << QString("  - one\n  - two") << QString("one\n  - two");
    QTest::newRow("quote-keeps-markdown") << QString("- one\n## two") << 0 << 12 << int(Editor::Quote)
                                          << QString("> - one\n> ## two") << QString("- one\n> ## two");
    QTest::newRow("code-block") << QString("one\ntwo\nnext") << 0 << 8 << int(Editor::CodeBlock)
                                << QString("```\none\ntwo\n```\nnext") << QString("one\ntwo");
    QTest::newRow("fence-in-code") << QString("```cpp\ncode\n```") << 0 << 15 << int(Editor::CodeBlock)
                                   << QString("code") << QString("code");
  }

  void selectionFormatting() {
    QFETCH(QString, source);
    QFETCH(int, start);
    QFETCH(int, end);
    QFETCH(int, format);
    QFETCH(QString, expected);
    QFETCH(QString, selected);
    using Editor = Vicinae::Editor::EditorDocument;
    QTextDocument text;
    Editor editor(m_repository);
    editor.attach(&text);
    editor.load("format", source);
    QSignalSpy selections(&editor, &Editor::selectionRequested);
    editor.format(start, end, static_cast<Editor::Format>(format));
    QCOMPARE(editor.text(), expected);
    QVERIFY(!selections.isEmpty());
    const auto range = selections.last();
    QCOMPARE(editor.snapshot(range[0].toInt(), range[1].toInt()).value("text").toString(), selected);
    text.undo();
    QCOMPARE(editor.text(), source);
    QVERIFY(!text.isUndoAvailable());
    text.redo();
    QCOMPARE(editor.text(), expected);
    if (format != Editor::CodeBlock) {
      editor.format(range[0].toInt(), range[1].toInt(), static_cast<Editor::Format>(format));
      QCOMPARE(editor.text(), source);
    }
  }

  void formattingLinksAndMixedBlocks() {
    using Editor = Vicinae::Editor::EditorDocument;
    QTextDocument text;
    Editor editor(m_repository);
    editor.attach(&text);
    QSignalSpy selections(&editor, &Editor::selectionRequested);
    editor.load("link", "a [label]");
    editor.format(0, 9, Editor::Link);
    QCOMPARE(editor.text(), "[a \\[label\\]](https://)");
    const auto range = selections.last();
    QCOMPARE(editor.snapshot(range[0].toInt(), range[1].toInt()).value("text").toString(), "https://");
    text.undo();
    QCOMPARE(editor.text(), "a [label]");
    editor.load("link-line", "label\nnext line");
    editor.format(0, 6, Editor::Link);
    QCOMPARE(editor.text(), "[label](https://)\nnext line");
    text.undo();
    QCOMPARE(editor.text(), "label\nnext line");
    for (const auto format :
         {Editor::Bold, Editor::Italic, Editor::StrikeThrough, Editor::InlineCode, Editor::Link}) {
      editor.load("blank-line", " \t\n\nnext line");
      editor.format(0, 4, format);
      QCOMPARE(editor.text(), " \t\n\nnext line");
      QVERIFY(!text.isUndoAvailable());
    }

    editor.load("mixed", "- [x] done\npending");
    editor.format(0, 18, Editor::TaskList);
    QCOMPARE(editor.text(), "- [x] done\n- [ ] pending");
    editor.load("heading", "## old\nplain");
    editor.format(0, 12, Editor::Heading1);
    QCOMPARE(editor.text(), "# old\n# plain");
    editor.load("fence", "some ``` ticks");
    editor.format(0, 14, Editor::CodeBlock);
    QCOMPARE(editor.text(), "````\nsome ``` ticks\n````");
    const auto codeRange = selections.last();
    editor.format(codeRange[0].toInt(), codeRange[1].toInt(), Editor::CodeBlock);
    QCOMPARE(editor.text(), "some ``` ticks");
    editor.format(-1, 5, Editor::Bold);
    editor.format(0, 100, Editor::Quote);
    QCOMPARE(editor.text(), "some ``` ticks");
  }

  void storageRoundTripsAndDoesNotResurrectDeletedNotes() {
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    Notes::Note note{"id", "A title", "# Unicode\n你好 👋\n```cpp\nint x;\n```\n", 100, 200};
    {
      Notes::Store store(directory.path().toStdString());
      auto saved = store.save(note, true).result();
      QVERIFY(saved);
      note = *saved;
      QVERIFY(!store.save(note, true).result());
      note.markdown += "A final edit";
      QVERIFY(store.save(note, false).result());
    }
    {
      Notes::Store store(directory.path().toStdString());
      const auto list = store.list().result();
      QVERIFY(list);
      QCOMPARE(list->size(), std::size_t(1));
      const auto loaded = store.load(list->front().id).result();
      QVERIFY(loaded);
      QCOMPARE(loaded->title, note.title);
      QCOMPARE(loaded->markdown, note.markdown);
      QVERIFY(QFileInfo::exists(directory.filePath("A title.md")));
      QVERIFY(store.remove(loaded->id).result());
      QVERIFY(!store.save(*loaded, false).result());
      QVERIFY(store.list().result()->empty());
    }
  }

  void sanitizedTitlesKeepTheirFileAndMissingFilesCanBeRemoved() {
    QTemporaryDir directory;
    Notes::Store store(directory.path().toStdString());
    auto saved = store.save({"id", "Q/A", "body"}, true).result();
    QVERIFY(saved);
    for (const auto *title : {"Q/A", "Q-A ", "Q-A."}) {
      saved->title = title;
      saved = store.save(*saved, false).result();
      QVERIFY(saved);
      QCOMPARE(saved->title, std::string("Q-A"));
    }
    QCOMPARE(QDir(directory.path()).entryList(QDir::Files), QStringList{"Q-A.md"});
    QVERIFY(QFile::remove(directory.filePath("Q-A.md")));
    QVERIFY(store.remove(saved->id).result());
    QVERIFY(!store.load(saved->id).result());
  }

  void directoryPreferencesUseNativePickers() {
    const auto schema = describePreferences<Notes::Preferences>();
    QCOMPARE(schema.size(), std::size_t(3));
    QVERIFY(!std::get<Preference::DirectoryPickerData>(schema[0].data()).multiple);
    QVERIFY(std::get<Preference::DirectoryPickerData>(schema[1].data()).multiple);

    const auto preferences = readPreferences<Notes::Preferences>(
        {{"defaultDirectory", "/notes"}, {"directories", glz::generic::array_t{"/reference", "/shared"}}});
    QCOMPARE(preferences.defaultDirectory, std::optional<std::string>("/notes"));
    QCOMPARE(preferences.directories, (std::vector<std::string>{"/reference", "/shared"}));
    QCOMPARE(preferences.fontSize, std::optional<int>());
    QCOMPARE(readPreferences<Notes::Preferences>({{"fontSize", 16.0}}).fontSize, std::optional<int>(16));
    QCOMPARE(readPreferences<Notes::Preferences>({{"fontSize", "18"}}).fontSize, std::optional<int>(18));
    QCOMPARE(readPreferences<Notes::Preferences>({{"fontSize", ""}}, {.fontSize = 18}).fontSize,
             std::optional<int>());
  }

  void directoriesAndExternalEdits() {
    QTemporaryDir data;
    QTemporaryDir first;
    QTemporaryDir second;
    Notes::Store store(data.path().toStdString(), {.defaultDirectory = second.path().toStdString(),
                                                   .directories = {first.path().toStdString()}});
    auto saved = store.save({"one", "Same title", "original"}, true).result();
    QVERIFY(saved);
    QCOMPARE(saved->path,
             (std::filesystem::path(Notes::Store::normalizeDirectory(second.path().toStdString())) /
              "Same title.md")
                 .string());
    QFile file(QString::fromStdString(saved->path));
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.write("external edit");
    file.close();
    QVERIFY(!store.save(*saved, false).result());
    auto loaded = store.load(saved->id).result();
    QVERIFY(loaded);
    QCOMPARE(loaded->markdown, std::string("external edit"));
    loaded->title = "Renamed";
    loaded = store.save(*loaded, false).result();
    QVERIFY(loaded);
    QVERIFY(!file.exists());
    QVERIFY(QFileInfo::exists(second.filePath("Renamed.md")));
    auto duplicate = store.save({"two", "Renamed", "another note"}, true).result();
    QVERIFY(duplicate);
    QVERIFY(duplicate->path != loaded->path);
    store.configure({.defaultDirectory = first.path().toStdString()});
    QVERIFY(store.list().result()->empty());
    QVERIFY(QFileInfo::exists(QString::fromStdString(loaded->path)));
    auto next = store.save({"three", "New note", "hello"}, true).result();
    QVERIFY(next);
    QCOMPARE(
        next->path,
        (std::filesystem::path(Notes::Store::normalizeDirectory(first.path().toStdString())) / "New note.md")
            .string());
    store.configure({.defaultDirectory = first.filePath("missing/nested").toStdString()});
    QVERIFY(store.save({"four", "Created", "hello"}, true).result());
    QVERIFY(QFileInfo::exists(first.filePath("missing/nested/Created.md")));
    store.configure({.defaultDirectory = first.filePath("New note.md/blocked").toStdString()});
    QVERIFY(!store.save({"five", "Unsaved", "hello"}, true).result());
  }

  void observesExternalFilesAndProtectsReadOnlyNotes() {
    QTemporaryDir directory;
    Notes::Store store(directory.path().toStdString());
    QVERIFY(store.list().result());
    QSignalSpy changed(&store, &Notes::Store::changed);
    QFile file(directory.filePath("External.md"));
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.write("external");
    file.close();
    QTRY_VERIFY(!changed.empty());
    auto notes = store.list().result();
    QVERIFY(notes);
    QCOMPARE(notes->size(), std::size_t(1));
    QVERIFY(file.setPermissions(QFileDevice::ReadOwner));
    if (QFileInfo(file).isWritable())
      QSKIP("Permissions do not restrict this user, as when running as root.");
    auto loaded = store.load(notes->front().id).result();
    QVERIFY(loaded);
    QVERIFY(loaded->readOnly);
    QVERIFY(!store.save(*loaded, false).result());
    QVERIFY(!store.remove(loaded->id).result());
    QVERIFY(file.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner));
  }

  void followsReplacedDirectorySymlinks() {
    QTemporaryDir directory;
    QDir().mkpath(directory.filePath("first"));
    QDir().mkpath(directory.filePath("second"));
    for (const auto &name : {QStringLiteral("first"), QStringLiteral("second")}) {
      QFile file(directory.filePath(name + "/Note.md"));
      QVERIFY(file.open(QIODevice::WriteOnly));
      file.write(name.toUtf8());
    }
    const auto link = directory.filePath("selected").toStdString();
    std::error_code error;
    std::filesystem::create_directory_symlink(directory.filePath("first").toStdString(), link, error);
    if (error) QSKIP("Directory symlinks are unavailable");
    Notes::Store store(directory.filePath("data").toStdString(), {.defaultDirectory = link});
    const auto before = store.list().result();
    QVERIFY(before);
    QCOMPARE(before->size(), std::size_t(1));
    QCOMPARE(before->front().markdown, std::string("first"));
    QSignalSpy changed(&store, &Notes::Store::changed);
    QVERIFY(std::filesystem::remove(link));
    std::filesystem::create_directory_symlink(directory.filePath("second").toStdString(), link);
    QTRY_VERIFY(!changed.empty());
    const auto after = store.list().result();
    QVERIFY(after);
    QCOMPARE(after->size(), std::size_t(1));
    QCOMPARE(after->front().id, before->front().id);
    QCOMPARE(after->front().markdown, std::string("second"));
    QVERIFY(!store.save(before->front(), false).result());
  }

  void storageErrorsRemainErrors() {
    QTemporaryDir directory;
    QFile file(directory.filePath("not-a-directory"));
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.close();
    Notes::Store store(file.fileName().toStdString());
    QVERIFY(!store.save({"id", "title", "body", 1, 1}, true).result());
    QVERIFY(!store.list().result());
  }

  void frontMatterTags() {
    using Tags = std::vector<std::string>;
    QCOMPARE(Notes::tags("---\ntags: [work, idea]\n---\nBody"), (Tags{"work", "idea"}));
    QCOMPARE(Notes::tags("---\ntitle: x\ntags:\n  - work\n  - \"#idea\"\nother: 1\n---\n"),
             (Tags{"work", "idea"}));
    QCOMPARE(Notes::tags("---\ntags: work, not a tag # comment\n---\n"), (Tags{"work"}));
    QVERIFY(Notes::tags("tags: [work]\n").empty());
    QCOMPARE(Notes::body("---\ntags: [work]\n---\nBody"), std::string_view("Body"));

    // A leading thematic break is not front matter.
    const std::string prose = "---\nSome text\n---\nBody";
    QCOMPARE(Notes::body(prose), std::string_view(prose));
    QVERIFY(Notes::tags("---\ntags: [work]\nBody").empty());

    const std::string plain = "# Title\n";
    const auto tagged = Notes::withTags(plain, Tags{"work", "a/b"});
    QCOMPARE(tagged, std::string("---\ntags: [work, a/b]\n---\n# Title\n"));
    QCOMPARE(Notes::withTags(tagged, Tags{}), plain);

    const std::string existing = "---\ntitle: x # keep\ntags:\n  - old\naliases: [y]\n---\n\nBody";
    QCOMPARE(Notes::withTags(existing, Tags{"new"}),
             std::string("---\ntitle: x # keep\ntags: [new]\naliases: [y]\n---\n\nBody"));
    QCOMPARE(Notes::withTags(existing, Tags{}),
             std::string("---\ntitle: x # keep\naliases: [y]\n---\n\nBody"));
    QCOMPARE(Notes::withTags("---\ntitle: x\n---\nBody", Tags{"new"}),
             std::string("---\ntitle: x\ntags: [new]\n---\nBody"));

    QVERIFY(Notes::isValidTag("work/2026_q1-été"));
    for (const auto *invalid : {"", "two words", "a,b", "[a]", "-a", "a:b", "#a"})
      QVERIFY2(!Notes::isValidTag(invalid), invalid);
  }

  void storeSetsTagsOnTheCurrentFile() {
    QTemporaryDir directory;
    Notes::Store store(directory.path().toStdString());
    const auto saved = store.save({.id = "id", .title = "Tagged", .markdown = "Body"}, true).result();
    QVERIFY(saved && saved->tags.empty());

    // Each change applies to what is on disk, so that queued changes need no fresh revision.
    store.setTags(saved->id, {"work"});
    const auto tagged = store.setTags(saved->id, {"work", "idea"}).result();
    QVERIFY(tagged);
    QCOMPARE(tagged->tags, (std::vector<std::string>{"work", "idea"}));
    QCOMPARE(tagged->markdown, std::string("---\ntags: [work, idea]\n---\nBody"));
    QCOMPARE(store.list().result()->front().tags, tagged->tags);
    QCOMPARE(store.setTags(saved->id, {}).result()->markdown, std::string("Body"));
    QVERIFY(!store.setTags("missing", {"work"}).result());
  }

  void toolsSearchReadAndWriteNotes() {
    QTemporaryDir directory;
    Notes::Store store(directory.path().toStdString());
    SearchNotesTool search(store);
    ReadNoteTool read(store);
    WriteNoteTool write(store);
    const auto run = [](AI::AbstractTool &tool, std::string_view arguments) {
      auto task = tool.runRaw(arguments);
      const bool finished = QTest::qWaitFor([&task] { return task.future.isFinished(); });
      return finished && task.future.resultCount() > 0
                 ? task.future.result()
                 : AI::AbstractTool::RawToolResult(std::unexpected(std::string("unfinished")));
    };
    const auto json = [](const AI::AbstractTool::RawToolResult &result) {
      glz::generic value;
      if (result) std::ignore = glz::read_json(value, result->content);
      return value;
    };
    const auto arguments = [](const glz::generic::object_t &value) {
      return glz::write_json(glz::generic(value)).value_or("");
    };

    QVERIFY(!run(write, R"({"content":"missing title"})"));
    const auto created = run(write, R"({"title":"Groceries","content":"- milk\n"})");
    QVERIFY(created);
    const auto path = json(created)["path"].get<std::string>();
    QCOMPARE(path, directory.filePath("Groceries.md").toStdString());

    const auto found = json(run(search, R"({"query":"grocer"})"));
    QCOMPARE(found.get<glz::generic::array_t>().size(), std::size_t(1));
    QCOMPARE(found[0]["path"].get<std::string>(), path);
    QCOMPARE(json(run(search, R"({"query":""})")).get<glz::generic::array_t>().size(), std::size_t(1));
    QVERIFY(json(run(search, R"({"query":"zzzzqqqq"})")).get<glz::generic::array_t>().empty());

    const auto first = json(run(read, arguments({{"path", path}})));
    QCOMPARE(first["content"].get<std::string>(), std::string("- milk\n"));
    const auto stale = first["revision"].get<std::string>();

    QFile outside(directory.filePath("secret.txt"));
    QVERIFY(outside.open(QIODevice::WriteOnly));
    outside.write("secret");
    outside.close();
    QVERIFY(!run(read, arguments({{"path", outside.fileName().toStdString()}})));
    QVERIFY(!run(write, arguments({{"path", outside.fileName().toStdString()}, {"content", "x"}})));

    const auto append = arguments({{"path", path}, {"mode", "append"}, {"content", "- eggs"}});
    QVERIFY(!write.requiresApproval(R"({"title":"New","content":""})"));
    QVERIFY(!write.requiresApproval(append));
    QVERIFY(run(write, append));
    QCOMPARE(store.list().result()->front().markdown, std::string("- milk\n- eggs"));

    const auto replace = [&](const std::string &revision) {
      return arguments(
          {{"path", path}, {"title", "Shopping"}, {"content", "- bread"}, {"revision", revision}});
    };
    QVERIFY(write.requiresApproval(replace(stale)));
    QVERIFY(!run(write, arguments({{"path", path}, {"content", "- bread"}})));
    QVERIFY(!run(write, replace(stale)));
    QCOMPARE(store.list().result()->front().markdown, std::string("- milk\n- eggs"));

    const auto current = json(run(read, arguments({{"path", path}})))["revision"].get<std::string>();
    const auto replaced = run(write, replace(current));
    QVERIFY(replaced);
    QCOMPARE(json(replaced)["path"].get<std::string>(), directory.filePath("Shopping.md").toStdString());
    const auto notes = store.list().result();
    QCOMPARE(notes->size(), std::size_t(1));
    QCOMPARE(notes->front().title, std::string("Shopping"));
    QCOMPARE(notes->front().markdown, std::string("- bread"));
  }

private:
  KSyntaxHighlighting::Repository m_repository;
};

QTEST_MAIN(NotesInvariantsTest)
#include "notes-invariants.moc"
