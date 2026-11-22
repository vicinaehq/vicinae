#include <QAbstractTextDocumentLayout>
#include <QAccessible>
#include <QDir>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QFile>
#include <QMimeData>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QQmlExtensionPlugin>
#include <QQuickItem>
#include <QQuickWindow>
#include <QScopeGuard>
#include <QSignalSpy>
#include <QStyleHints>
#include <QTemporaryDir>
#include <QTest>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextLayout>
#include "editor-document.hpp"
#include "services/notes/notes-store.hpp"
#include "ui/quick/syntax-highlighter.hpp"

Q_IMPORT_QML_PLUGIN(Vicinae_EditorPlugin)

class NotesInvariantsTest : public QObject {
  Q_OBJECT

private slots:
  void nativeDropsFollowThePointerAndRespectReadOnly() {
    QQmlEngine engine;
    Vicinae::Editor::EditorDocument editor(m_repository);
    QQuickWindow window;
    window.resize(800, 600);
    QQmlComponent component(&engine, QUrl(QStringLiteral("qrc:/qt/qml/Vicinae/Editor/MarkdownEditor.qml")));
    auto *item = qobject_cast<QQuickItem *>(
        component.createWithInitialProperties({{"document", QVariant::fromValue(&editor)}}));
    QVERIFY2(item, qPrintable(component.errorString()));
    item->setParent(&window);
    item->setParentItem(window.contentItem());
    item->setSize(QSizeF(800, 600));
    auto *edit = qobject_cast<QQuickItem *>(editor.textDocument()->parent());
    auto *area = item->findChild<QQuickItem *>("markdown-drop-area");
    auto *viewport = item->property("flickable").value<QQuickItem *>();
    QVERIFY(edit && area && viewport);
    const QString original = QStringLiteral("Before 👋\nTarget line\nAfter\n").repeated(100);
    editor.load("drop", original);
    window.show();
    QVERIFY(QTest::qWaitForWindowExposed(&window));
    viewport->setProperty("contentY", 500);
    QCoreApplication::processEvents();
    const QPoint point(120, 250);
    const auto local = edit->mapFromScene(point);
    int position = -1;
    QVERIFY(QMetaObject::invokeMethod(edit, "positionAt", Q_RETURN_ARG(int, position),
                                      Q_ARG(double, local.x()), Q_ARG(double, local.y())));
    QVERIFY(position > 0);
    QMimeData text;
    text.setText("Dropped text");
    QDragEnterEvent enter(point, Qt::CopyAction | Qt::MoveAction, &text, Qt::LeftButton, Qt::NoModifier);
    QCoreApplication::sendEvent(&window, &enter);
    QVERIFY(enter.isAccepted());
    QCOMPARE(enter.dropAction(), Qt::CopyAction);
    QVERIFY(area->property("containsDrag").toBool());
    QCOMPARE(area->property("insertionPosition").toInt(), position);
    QDropEvent drop(point, Qt::CopyAction | Qt::MoveAction, &text, Qt::LeftButton, Qt::NoModifier);
    QCoreApplication::sendEvent(&window, &drop);
    QVERIFY(drop.isAccepted());
    QCOMPARE(drop.dropAction(), Qt::CopyAction);
    QCOMPARE(editor.text(), QString(original).insert(position, text.text()));
    QCOMPARE(edit->property("cursorPosition").toInt(), position + text.text().size());
    QVERIFY(!area->property("containsDrag").toBool());
    editor.textDocument()->textDocument()->undo();
    QCOMPARE(editor.text(), original);

    QDragEnterEvent edge(QPoint(120, 595), Qt::CopyAction, &text, Qt::LeftButton, Qt::NoModifier);
    QCoreApplication::sendEvent(&window, &edge);
    QVERIFY(edge.isAccepted());
    const auto beforeScroll = viewport->property("contentY").toReal();
    const auto beforePosition = area->property("insertionPosition").toInt();
    QTRY_VERIFY(viewport->property("contentY").toReal() > beforeScroll + 40);
    QVERIFY(area->property("insertionPosition").toInt() > beforePosition);
    QDragLeaveEvent leave;
    QCoreApplication::sendEvent(&window, &leave);
    QVERIFY(!area->property("containsDrag").toBool());

    item->setProperty("readOnly", true);
    QDragEnterEvent readOnly(point, Qt::CopyAction, &text, Qt::LeftButton, Qt::NoModifier);
    QCoreApplication::sendEvent(&window, &readOnly);
    QVERIFY(!readOnly.isAccepted());
    QCoreApplication::sendEvent(&window, &leave);
    QCOMPARE(editor.text(), original);
    item->setProperty("readOnly", false);
    QDragEnterEvent moveOnly(point, Qt::MoveAction, &text, Qt::LeftButton, Qt::NoModifier);
    QCoreApplication::sendEvent(&window, &moveOnly);
    QVERIFY(!moveOnly.isAccepted());
    QCoreApplication::sendEvent(&window, &leave);

    QImage image(8, 8, QImage::Format_ARGB32);
    image.fill(Qt::blue);
    QMimeData imageMime;
    imageMime.setImageData(image);
    QVERIFY(!editor.acceptsDrop(&imageMime));
    bool received = false;
    editor.setDropHandler([](const QMimeData *mime) { return mime->hasImage(); },
                          [&](const QMimeData *mime, int at) {
                            received = qvariant_cast<QImage>(mime->imageData()) == image && at > 0;
                            return true;
                          });
    QDragEnterEvent imageEnter(point, Qt::CopyAction, &imageMime, Qt::LeftButton, Qt::NoModifier);
    QCoreApplication::sendEvent(&window, &imageEnter);
    QVERIFY(imageEnter.isAccepted());
    QDropEvent imageDrop(point, Qt::CopyAction, &imageMime, Qt::LeftButton, Qt::NoModifier);
    QCoreApplication::sendEvent(&window, &imageDrop);
    QVERIFY(imageDrop.isAccepted());
    QVERIFY(received);
    QCOMPARE(editor.text(), original);
  }

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

  void cursorReappearsAfterScrollingAndClicking() {
    auto *styleHints = QGuiApplication::styleHints();
    const auto flashTime = styleHints->cursorFlashTime();
    styleHints->setCursorFlashTime(0);
    const auto restoreFlashTime = qScopeGuard([&] { styleHints->setCursorFlashTime(flashTime); });
    QQmlEngine engine;
    Vicinae::Editor::EditorDocument editor(m_repository);
    QQuickWindow window;
    window.resize(800, 600);
    window.setColor(Qt::white);
    QQmlComponent component(&engine, QUrl(QStringLiteral("qrc:/qt/qml/Vicinae/Editor/MarkdownEditor.qml")));
    auto *item = qobject_cast<QQuickItem *>(
        component.createWithInitialProperties({{"document", QVariant::fromValue(&editor)}}));
    QVERIFY2(item, qPrintable(component.errorString()));
    item->setParent(&window);
    item->setParentItem(window.contentItem());
    item->setSize(QSizeF(800, 600));
    item->setProperty("foreground", QColor(Qt::black));
    auto *textItem = qobject_cast<QQuickItem *>(editor.textDocument()->parent());
    auto *viewport = item->property("flickable").value<QQuickItem *>();
    QVERIFY(textItem);
    QVERIFY(viewport);
    editor.load("scroll", QStringLiteral("A line of text\n\n").repeated(1000));
    window.show();
    window.requestActivate();
    QVERIFY(QTest::qWaitForWindowActive(&window));
    textItem->forceActiveFocus();
    QTRY_VERIFY(textItem->hasActiveFocus());

    auto cursorIsPainted = [&] {
      const auto rect = textItem->mapRectToScene(textItem->property("cursorRectangle").toRectF());
      const auto image = window.grabWindow();
      if (image.isNull()) return false;
      const auto scale = image.devicePixelRatio();
      const QRect pixels(QPoint(qRound(rect.x() * scale), qRound(rect.y() * scale)),
                         QSize(qCeil(rect.width() * scale), qCeil(rect.height() * scale)));
      int darkPixels = 0;
      for (int y = pixels.top(); y <= pixels.bottom(); ++y)
        for (int x = pixels.left(); x <= pixels.right(); ++x)
          if (image.rect().contains(x, y) && image.pixelColor(x, y).lightness() < 100) ++darkPixels;
      return darkPixels >= 10;
    };
    QTRY_VERIFY(cursorIsPainted());
    viewport->setProperty("contentY", 1600);
    QTRY_VERIFY(textItem->clipRect().top() > 1500);
    window.grabWindow();
    QTest::mouseClick(&window, Qt::LeftButton, Qt::NoModifier, QPoint(600, 300));
    const auto position = textItem->property("cursorPosition").toInt();
    QVERIFY(position > 0);
    QTRY_VERIFY(cursorIsPainted());
    QTest::keyClick(&window, Qt::Key_X);
    QCOMPARE(editor.text().at(position), QChar('x'));
    QTRY_VERIFY(cursorIsPainted());

    viewport->setProperty("contentY", 0);
    QTRY_VERIFY(!textItem->property("cursorVisible").toBool());
    QQuickItem otherControl(window.contentItem());
    otherControl.forceActiveFocus();
    QTRY_VERIFY(!textItem->hasActiveFocus());
    textItem->forceActiveFocus();
    QTRY_VERIFY(textItem->hasActiveFocus());
    viewport->setProperty("contentY", 100);
    window.grabWindow();
    QTest::mouseClick(&window, Qt::LeftButton, Qt::NoModifier, QPoint(600, 100));
    QVERIFY(textItem->property("cursorPosition").toInt() < position);
    QTRY_VERIFY(cursorIsPainted());
    otherControl.forceActiveFocus();
    QTRY_VERIFY(!textItem->hasActiveFocus());
    QTRY_VERIFY(!textItem->property("cursorVisible").toBool());
    textItem->forceActiveFocus();
    QTRY_VERIFY(cursorIsPainted());
  }

  void accessibilityDoesNotEnableFullDocumentLayoutOnEveryEdit() {
    const bool accessibilityWasActive = QAccessible::isActive();
    QAccessible::setActive(true);
    const auto previousHandler = QAccessible::installUpdateHandler([](QAccessibleEvent *event) {
      auto inspect = [](auto &&self, QAccessibleInterface *accessible) -> void {
        if (!accessible) return;
        accessible->rect();
        for (int i = 0; i < accessible->childCount(); ++i)
          self(self, accessible->child(i));
      };
      auto *accessible = event->accessibleInterface();
      if (auto *item = qobject_cast<QQuickItem *>(event->object()); item && item->window())
        accessible = QAccessible::queryAccessibleInterface(item->window());
      inspect(inspect, accessible);
    });
    const auto restoreAccessibility = qScopeGuard([&] {
      QAccessible::installUpdateHandler(previousHandler);
      QAccessible::setActive(accessibilityWasActive);
    });

    QQmlEngine engine;
    Vicinae::Editor::EditorDocument editor(m_repository);
    QQmlComponent component(&engine);
    component.setData(R"(
      import QtQuick
      import QtQuick.Controls
      import Vicinae.Editor
      Window {
        id: window
        required property MarkdownEditorDocument document
        width: 800
        height: 600
        TextField { Accessible.name: qsTr("Note title") }
        MarkdownEditor { anchors.fill: parent; document: window.document }
      }
    )",
                      QUrl());
    auto *window = qobject_cast<QQuickWindow *>(
        component.createWithInitialProperties({{"document", QVariant::fromValue(&editor)}}));
    QVERIFY2(window, qPrintable(component.errorString()));
    const auto cleanup = qScopeGuard([&] { window->deleteLater(); });
    window->show();
    auto *textItem = qobject_cast<QQuickItem *>(editor.textDocument()->parent());
    QVERIFY(textItem);
    auto *text = editor.textDocument()->textDocument();
    const auto source = QStringLiteral("## Section\n\n**Bold** and `code`\n\n").repeated(4000);
    editor.load("large", source);
    QTRY_VERIFY(textItem->height() > window->height() * 100);
    auto *accessible = QAccessible::queryAccessibleInterface(textItem);
    QVERIFY(accessible);
    QCOMPARE(accessible->text(QAccessible::Name), "Markdown editor");
    QVERIFY(!accessible->rect().isEmpty());

    bool measuredUnwrappedWidth = false;
    QObject observer;
    connect(text->documentLayout(), &QAbstractTextDocumentLayout::update, &observer,
            [&](const QRectF &) { measuredUnwrappedWidth |= text->textWidth() < 0; });
    textItem->forceActiveFocus();
    QTest::keyClick(window, Qt::Key_X);
    QTRY_COMPARE(editor.text(), "x" + source);
    QVERIFY(!measuredUnwrappedWidth);
    QTest::keyClick(window, Qt::Key_Return);
    QTRY_COMPARE(editor.text(), "x\n" + source);
    QVERIFY(!measuredUnwrappedWidth);
    text->undo();
    QCOMPARE(editor.text(), "x" + source);
    text->undo();
    QCOMPARE(editor.text(), source);
    QVERIFY(!measuredUnwrappedWidth);
  }

  void largeEditorKeepsRenderingWithinTheViewport() {
    QQmlEngine engine;
    Vicinae::Editor::EditorDocument editor(m_repository);
    QQuickWindow window;
    window.resize(800, 600);
    QQmlComponent component(&engine, QUrl(QStringLiteral("qrc:/qt/qml/Vicinae/Editor/MarkdownEditor.qml")));
    auto *item = qobject_cast<QQuickItem *>(
        component.createWithInitialProperties({{"document", QVariant::fromValue(&editor)}}));
    QVERIFY2(item, qPrintable(component.errorString()));
    item->setParent(&window);
    item->setParentItem(window.contentItem());
    item->setSize(QSizeF(800, 600));
    QVERIFY(editor.textDocument());
    auto *textItem = qobject_cast<QQuickItem *>(editor.textDocument()->parent());
    auto *viewport = item->property("flickable").value<QQuickItem *>();
    QVERIFY(textItem);
    QVERIFY(viewport);
    window.show();

    const auto source = QStringLiteral("## Section\n\n**Bold** and `code`\n\n").repeated(4000);
    editor.load("large", source);
    QTRY_VERIFY(textItem->height() > window.height() * 100);
    QVERIFY(textItem->flags().testFlag(QQuickItem::ItemObservesViewport));
    QCOMPARE(textItem->viewportItem(), viewport);
    QVERIFY(textItem->clipRect().height() <= viewport->height());

    const auto scrollTo = viewport->property("contentHeight").toReal() / 2;
    viewport->setProperty("contentY", scrollTo);
    QTRY_VERIFY(textItem->clipRect().top() > scrollTo - 100);
    QVERIFY(textItem->clipRect().height() <= viewport->height());
    QCOMPARE(editor.text(), source);

    editor.load("short", "A short note");
    editor.load("large", source);
    QTRY_COMPARE(viewport->property("contentY").toReal(), 0);
    QVERIFY(textItem->clipRect().height() <= viewport->height());
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

  void markdownUsesSemanticRolesAndPreservesEmbeddedCode() {
    using namespace Vicinae;
    QTextDocument text;
    Editor::EditorDocument editor(m_repository);
    editor.attach(&text);
    const QString source = QStringLiteral(
        "# Heading\n\n**bold** and *italic* and ~~removed~~ and `inline`\n\n"
        "![Alt text](https://example.com/image.png)\n![Local](image.png)\n![Reference][image]\n"
        "[Label](https://example.com)\n\n> Quote\n\n- [x] Complete\n\n"
        "```cpp\nconst char *message = \"hello\"; // *literal*\n```\n");
    editor.load("styles", source);
    QTRY_VERIFY(!text.begin().layout()->formats().isEmpty());
    auto formatAt = [&](const QString &needle, int inside = 0) {
      const auto position = source.indexOf(needle) + inside;
      const auto block = text.findBlock(position);
      for (const auto &range : block.layout()->formats()) {
        if (position - block.position() >= range.start &&
            position - block.position() < range.start + range.length)
          return range.format;
      }
      return QTextCharFormat{};
    };
    for (bool dark : {true, false}) {
      Syntax::Palette palette;
      palette.foreground = dark ? QColor("#c7c9ce") : QColor("#30343a");
      palette.background = dark ? QColor("#24262a") : QColor("#f7f5f2");
      palette.marker = QColor("#5678ab");
      editor.setPalette(palette);
      QCOMPARE(formatAt("Heading").foreground().color(), palette.readable(palette.heading));
      QCOMPARE(formatAt("Heading").fontWeight(), QFont::Bold);
      QCOMPARE(formatAt("#").foreground().color(), palette.readable(palette.marker));
      QCOMPARE(formatAt("bold").fontWeight(), QFont::Bold);
      QCOMPARE(formatAt("**").fontWeight(), QFont::Normal);
      QCOMPARE(formatAt("**").foreground().color(), palette.readable(palette.emphasis));
      QVERIFY(formatAt("italic").fontItalic());
      QVERIFY(formatAt("removed").fontStrikeOut());
      QCOMPARE(formatAt("inline").foreground().color(), palette.readable(palette.inlineCode));
      QVERIFY(formatAt("inline").foreground().color() != formatAt("hello").foreground().color());
      for (const auto &label : {"Alt text", "Local", "Reference", "Label"})
        QCOMPARE(formatAt(label).foreground().color(), palette.readable(palette.link));
      QCOMPARE(formatAt("Quote").foreground().color(), palette.foreground);
      QCOMPARE(formatAt("https://example.com/image.png").foreground().color(),
               palette.readable(palette.linkTarget));
      QCOMPARE(formatAt("(image.png)", 1).foreground().color(), palette.readable(palette.linkTarget));
      QCOMPARE(formatAt("![").foreground().color(), palette.readable(palette.marker));
      QCOMPARE(formatAt("const").foreground().color(), palette.styles()[Syntax::TextStyle::Attribute]);
      QCOMPARE(formatAt("const").fontWeight(), QFont::Normal);
      QCOMPARE(formatAt("char").foreground().color(), palette.styles()[Syntax::TextStyle::DataType]);
      QCOMPARE(formatAt("hello").foreground().color(), palette.styles()[Syntax::TextStyle::String]);
      QCOMPARE(formatAt("literal").foreground().color(), palette.readable(palette.comment));
      QCOMPARE(editor.text(), source);
      QCOMPARE(editor.revision(), 0);
      QVERIFY(!text.isUndoAvailable());
    }
  }

  void previewCodeUsesTheSamePaletteAndEscapesMarkup() {
    const Vicinae::Syntax::Palette palette;
    const auto styles = palette.styles();
    const QString source = QStringLiteral("const char *value = \"<strong>hello</strong>\";");
    QTextDocument preview;
    preview.setHtml(syntax::highlight(source, "C++", styles, true, "monospace"));
    QCOMPARE(preview.toPlainText(), source);
    QTextCursor cursor(&preview);
    cursor.setPosition(2);
    QCOMPARE(cursor.charFormat().foreground().color().name(),
             styles[Vicinae::Syntax::TextStyle::Attribute].name());
    QCOMPARE(cursor.charFormat().fontWeight(), QFont::Normal);
    cursor.setPosition(source.indexOf("hello") + 1);
    QCOMPARE(cursor.charFormat().foreground().color().name(),
             styles[Vicinae::Syntax::TextStyle::String].name());
    QVERIFY(!preview.isUndoAvailable());
  }

  void syntaxColorsRemainReadableInLightAndDarkThemes() {
    using namespace Vicinae::Syntax;
    for (bool dark : {true, false}) {
      Palette palette;
      palette.background = dark ? QColor("#16171b") : QColor("#fcfaf7");
      palette.foreground = dark ? QColor("#dadce0") : QColor("#282a30");
      palette.keyword = Qt::blue;
      palette.controlFlow = Qt::magenta;
      palette.string = Qt::green;
      palette.type = Qt::cyan;
      palette.number = Qt::yellow;
      palette.error = Qt::red;
      for (const auto &color : palette.styles()) {
        QVERIFY(contrast(color, palette.background) >= 4.49);
        QVERIFY(color.isValid());
      }
      if (contrast(palette.string, palette.background) >= 4.5)
        QCOMPARE(palette.styles()[TextStyle::String], palette.string);
      QCOMPARE(palette.styles()[TextStyle::Variable], palette.foreground);
    }
  }

  void syntaxRolesUseThemeOverridesAndFallbacks() {
    auto parsed = ThemeFile::fromString(R"(
      [meta]
      name = "Custom"
      description = "Syntax palette"
      variant = "dark"
      [colors.accents]
      blue = "#98b8e8"
      orange = "#e8b898"
      [colors.syntax]
      keyword = "colors.accents.orange"
      inline_code = "#e8c898"
    )",
                                        "custom.toml");
    QVERIFY(parsed);
    QVERIFY(parsed->diagnostics.empty());
    parsed->file.setParent(std::make_shared<ThemeFile>(ThemeFile::vicinaeDark()));
    const auto palette = syntax::paletteForTheme(parsed->file);
    QCOMPARE(palette.keyword, QColor("#e8b898"));
    QCOMPARE(palette.marker, QColor("#98b8e8"));
    QCOMPARE(palette.inlineCode, QColor("#e8c898"));
    QCOMPARE(palette.function, parsed->file.resolve(SemanticColor::Yellow));
    const auto restored = ThemeFile::fromString(parsed->file.toToml(), "restored.toml");
    QVERIFY(restored);
    QVERIFY(restored->diagnostics.empty());
    const auto roundTrip = syntax::paletteForTheme(restored->file);
    QCOMPARE(roundTrip.keyword, palette.keyword);
    QCOMPARE(roundTrip.inlineCode, palette.inlineCode);
    QCOMPARE(roundTrip.marker, palette.marker);
  }

  void bundledSyntaxPalettesRemainReadable() {
    for (const auto &entry : std::filesystem::directory_iterator(THEME_DIR)) {
      if (entry.path().extension() != ".toml") continue;
      const auto path = entry.path().string();
      auto parsed = ThemeFile::fromFile(entry.path());
      QVERIFY2(parsed.has_value(), path.c_str());
      for (const auto &diagnostic : parsed->diagnostics)
        QVERIFY2(!diagnostic.starts_with("unused config key colors.syntax"), diagnostic.c_str());
      parsed->file.setParent(std::make_shared<ThemeFile>(parsed->file.isDark() ? ThemeFile::vicinaeDark()
                                                                               : ThemeFile::vicinaeLight()));
      const auto palette = syntax::paletteForTheme(parsed->file);
      for (const auto &color : palette.styles()) {
        QVERIFY2(color.isValid(), path.c_str());
        QVERIFY2(Vicinae::Syntax::contrast(color, palette.background) >=
                     std::min(4.5, Vicinae::Syntax::contrast(palette.foreground, palette.background)) - 0.01,
                 path.c_str());
      }
    }
    for (const auto &theme : {ThemeFile::vicinaeDark(), ThemeFile::vicinaeLight()}) {
      const auto palette = syntax::paletteForTheme(theme);
      const auto styles = palette.styles();
      QCOMPARE(styles[Vicinae::Syntax::TextStyle::Keyword], palette.keyword);
      QCOMPARE(styles[Vicinae::Syntax::TextStyle::Function], palette.function);
      QCOMPARE(styles[Vicinae::Syntax::TextStyle::String], palette.string);
      QVERIFY(styles[Vicinae::Syntax::TextStyle::Keyword] != styles[Vicinae::Syntax::TextStyle::ControlFlow]);
      QVERIFY(styles[Vicinae::Syntax::TextStyle::Keyword] != styles[Vicinae::Syntax::TextStyle::Attribute]);
    }
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

  void selectionToolbarPreservesFocusAndUndo() {
    QQmlEngine engine;
    Vicinae::Editor::EditorDocument editor(m_repository);
    QQuickWindow window;
    window.resize(800, 600);
    QQmlComponent component(&engine, QUrl(QStringLiteral("qrc:/qt/qml/Vicinae/Editor/MarkdownEditor.qml")));
    auto *item = qobject_cast<QQuickItem *>(
        component.createWithInitialProperties({{"document", QVariant::fromValue(&editor)}}));
    QVERIFY2(item, qPrintable(component.errorString()));
    item->setParent(&window);
    item->setParentItem(window.contentItem());
    item->setSize(QSizeF(800, 600));
    auto *edit = qobject_cast<QQuickItem *>(editor.textDocument()->parent());
    auto *toolbar = item->findChild<QQuickItem *>("markdown-selection-toolbar");
    QVERIFY(edit);
    QVERIFY(toolbar);
    const auto action = [toolbar](QStringView label) -> QQuickItem * {
      const auto find = [label](auto &&self, QQuickItem *item) -> QQuickItem * {
        if (item->property("label").toString() == label) return item;
        for (auto *child : item->childItems())
          if (auto *match = self(self, child)) return match;
        return nullptr;
      };
      return find(find, toolbar);
    };
    auto *bold = action(u"Bold");
    QVERIFY(bold);
    editor.load("toolbar", "hello world\n\n" + QStringLiteral("Another line\n").repeated(1000));
    window.show();
    window.requestActivate();
    QVERIFY(QTest::qWaitForWindowActive(&window));
    edit->forceActiveFocus();
    QMetaObject::invokeMethod(edit, "select", Q_ARG(int, 0), Q_ARG(int, 5));
    QTRY_VERIFY(toolbar->isVisible());
    QVERIFY(toolbar->x() >= 0 && toolbar->x() + toolbar->width() <= item->width());
    QVERIFY(toolbar->y() >= 0 && toolbar->y() + toolbar->height() <= item->height());
    QTest::mouseClick(&window, Qt::LeftButton, Qt::NoModifier,
                      bold->mapToScene(bold->boundingRect().center()).toPoint());
    QTRY_VERIFY(editor.text().startsWith("**hello** world"));
    QVERIFY(edit->hasActiveFocus());
    QCOMPARE(edit->property("selectedText").toString(), "hello");
    editor.textDocument()->textDocument()->undo();
    QVERIFY(editor.text().startsWith("hello world"));
    QMetaObject::invokeMethod(edit, "select", Q_ARG(int, 0), Q_ARG(int, 5));
    QTRY_VERIFY(toolbar->isVisible());
    QTest::keyClick(&window, Qt::Key_Escape);
    QTRY_VERIFY(!toolbar->isVisible());
    QCOMPARE(edit->property("selectedText").toString(), "hello");
    QMetaObject::invokeMethod(edit, "select", Q_ARG(int, 6), Q_ARG(int, 11));
    QTRY_VERIFY(toolbar->isVisible());
    item->setWidth(220);
    QTRY_VERIFY(toolbar->x() + toolbar->width() <= 220);
    QTRY_VERIFY(toolbar->height() > 60);
    for (const auto label : {u"Heading", u"Bold", u"Italic", u"Strikethrough", u"Link", u"Inline Code",
                             u"Code Block", u"Bulleted List", u"Numbered List", u"Task List", u"Quote"}) {
      auto *button = action(label);
      QVERIFY(button);
      QVERIFY(button->isVisible());
      const auto rect = button->mapRectToItem(toolbar, button->boundingRect());
      QVERIFY(toolbar->boundingRect().contains(rect));
    }
    auto *viewport = item->property("flickable").value<QQuickItem *>();
    viewport->setProperty("contentY", 1600);
    QTRY_VERIFY(!toolbar->isVisible());
    viewport->setProperty("contentY", 0);
    QTRY_VERIFY(toolbar->isVisible());
    QTest::keyClick(&window, Qt::Key_X);
    QTRY_VERIFY(!toolbar->isVisible());
    QVERIFY(editor.text().startsWith("hello x"));

    editor.load("toolbar", "selected text");
    QMetaObject::invokeMethod(edit, "select", Q_ARG(int, 0), Q_ARG(int, 8));
    QTRY_VERIFY(toolbar->isVisible());
    auto *taskAction = action(u"Task List");
    QVERIFY(taskAction);
    QTest::mouseClick(&window, Qt::LeftButton, Qt::NoModifier,
                      taskAction->mapToScene(taskAction->boundingRect().center()).toPoint());
    QTRY_COMPARE(editor.text(), "- [ ] selected text");
    QTRY_VERIFY(edit->hasActiveFocus());
    QCOMPARE(edit->property("selectedText").toString(), "selected");
    editor.textDocument()->textDocument()->undo();
    QCOMPARE(editor.text(), "selected text");

    editor.load("line-selection", "selected line\nnext line");
    QMetaObject::invokeMethod(edit, "select", Q_ARG(int, 0), Q_ARG(int, 14));
    QTRY_VERIFY(toolbar->isVisible());
    QTest::mouseClick(&window, Qt::LeftButton, Qt::NoModifier,
                      bold->mapToScene(bold->boundingRect().center()).toPoint());
    QTRY_COMPARE(editor.text(), "**selected line**\nnext line");
    QCOMPARE(edit->property("selectedText").toString(), "selected line");
    QTest::keySequence(&window, QKeySequence::Undo);
    QCOMPARE(editor.text(), "selected line\nnext line");

    QMetaObject::invokeMethod(edit, "select", Q_ARG(int, 0), Q_ARG(int, 14));
    auto *heading = action(u"Heading");
    QVERIFY(heading);
    QTest::mouseClick(&window, Qt::LeftButton, Qt::NoModifier,
                      heading->mapToScene(heading->boundingRect().center()).toPoint());
    QTRY_VERIFY(toolbar->property("headingsVisible").toBool());
    auto *heading2 = action(u"Heading 2");
    QVERIFY(heading2);
    QVERIFY(heading2->isVisible());
    QTest::mouseClick(&window, Qt::LeftButton, Qt::NoModifier,
                      heading2->mapToScene(heading2->boundingRect().center()).toPoint());
    QTRY_COMPARE(editor.text(), "## selected line\nnext line");
    QVERIFY(!toolbar->property("headingsVisible").toBool());
    QVERIFY(edit->hasActiveFocus());
    editor.textDocument()->textDocument()->undo();
    QCOMPARE(editor.text(), "selected line\nnext line");

    item->setWidth(410);
    editor.load("resize", "A native Markdown note\n\nSelect this line\nFollowing line");
    QMetaObject::invokeMethod(edit, "select", Q_ARG(int, 24), Q_ARG(int, 40));
    QTRY_VERIFY(toolbar->isVisible());
    window.grabWindow();
    item->setWidth(220);
    QTRY_VERIFY(toolbar->height() > 60);
    window.grabWindow();
    const auto toolbarClearsSelection = [&] {
      QRectF selectionStart;
      QMetaObject::invokeMethod(edit, "positionToRectangle", Q_RETURN_ARG(QRectF, selectionStart),
                                Q_ARG(int, 24));
      const auto selectionEnd = edit->property("cursorRectangle").toRectF();
      const auto offset = edit->y() - viewport->property("contentY").toReal();
      return toolbar->y() + toolbar->height() <= selectionStart.top() + offset ||
             toolbar->y() >= selectionEnd.bottom() + offset;
    };
    QTRY_VERIFY(toolbarClearsSelection());
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
    QCOMPARE(schema.size(), std::size_t(2));
    QVERIFY(!std::get<Preference::DirectoryPickerData>(schema[0].data()).multiple);
    QVERIFY(std::get<Preference::DirectoryPickerData>(schema[1].data()).multiple);

    const auto preferences = readPreferences<Notes::Preferences>(
        {{"defaultDirectory", "/notes"}, {"directories", glz::generic::array_t{"/reference", "/shared"}}});
    QCOMPARE(preferences.defaultDirectory, std::optional<std::string>("/notes"));
    QCOMPARE(preferences.directories, (std::vector<std::string>{"/reference", "/shared"}));
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
    store.configure({.defaultDirectory = first.filePath("missing").toStdString()});
    QVERIFY(!store.save({"four", "Unsaved", "hello"}, true).result());
    QVERIFY(!QFileInfo::exists(first.filePath("missing")));
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

private:
  KSyntaxHighlighting::Repository m_repository;
};

QTEST_MAIN(NotesInvariantsTest)
#include "notes-invariants.moc"
