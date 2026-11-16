#include <QAbstractTextDocumentLayout>
#include <QAccessible>
#include <QFile>
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
  void cursorReappearsAfterScrollingAndClicking() {
    auto *styleHints = QGuiApplication::styleHints();
    const auto flashTime = styleHints->cursorFlashTime();
    styleHints->setCursorFlashTime(0);
    const auto restoreFlashTime = qScopeGuard([&] { styleHints->setCursorFlashTime(flashTime); });
    QQmlEngine engine;
    Vicinae::Editor::EditorDocument editor;
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
    Vicinae::Editor::EditorDocument editor;
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
    Vicinae::Editor::EditorDocument editor;
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
    Vicinae::Editor::EditorDocument editor;
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
    Editor::EditorDocument editor;
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
    Vicinae::Editor::EditorDocument editor;
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

  void listContinuationHasOneUndoStep() {
    QTextDocument text;
    Vicinae::Editor::EditorDocument editor;
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
    Vicinae::Editor::EditorDocument editor;
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

  void storageRoundTripsAndDoesNotResurrectDeletedNotes() {
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    Notes::Note note{"id", "A title", "# Unicode\n你好 👋\n```cpp\nint x;\n```\n", 100, 200};
    {
      Notes::Store store(directory.path().toStdString());
      QVERIFY(store.save(note, true).result());
      QVERIFY(!store.save(note, true).result());
      note.markdown += "A final edit";
      note.updatedAt = 300;
      QVERIFY(store.save(note, false).result());
    }
    {
      Notes::Store store(directory.path().toStdString());
      const auto loaded = store.load(note.id).result();
      QVERIFY(loaded);
      QCOMPARE(loaded->title, note.title);
      QCOMPARE(loaded->markdown, note.markdown);
      QCOMPARE(loaded->createdAt, note.createdAt);
      const auto list = store.list().result();
      QVERIFY(list);
      QCOMPARE(list->size(), std::size_t(1));
      QVERIFY(store.remove(note.id).result());
      QVERIFY(!store.save(note, false).result());
      QVERIFY(store.list().result()->empty());
    }
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
};

QTEST_MAIN(NotesInvariantsTest)
#include "notes-invariants.moc"
