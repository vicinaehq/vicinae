#pragma once
#include "script/script-output-tokenizer.hpp"
#include "ui/omni-painter/omni-painter.hpp"
#include "ui/scroll-bar/scroll-bar.hpp"
#include <qtextbrowser.h>
#include <qtextcursor.h>

class ScriptOutputTextDocument : public QTextDocument {
public:
  ScriptOutputTextDocument() = default;

  void setScriptText(const QString &text) {
    QTextCursor cursor(this);
    QTextBlockFormat bfmt = cursor.blockFormat();
    QTextCharFormat defaultCharFmt = cursor.charFormat();

    cursor.movePosition(QTextCursor::MoveOperation::StartOfBlock);

    defaultCharFmt.setFontPointSize(10.5);
    cursor.setCharFormat(defaultCharFmt);

    ScriptOutputTokenizer tokenizer(text);

    while (const auto tok = tokenizer.next()) {
      if (const auto &fmt = tok->fmt) {
        QTextCharFormat newFormat = cursor.charFormat();

        newFormat.setFontUnderline(fmt->underline);
        newFormat.setFontItalic(fmt->italic);

        if (fmt->reset) newFormat = defaultCharFmt;
        if (fmt->fg) { newFormat.setForeground(*fmt->fg); }
        if (fmt->bg) { newFormat.setBackground(*fmt->bg); }

        cursor.setCharFormat(newFormat);
      }

      QTextCharFormat old = cursor.charFormat();

      if (tok->url && QUrl(tok->text).isValid()) {
        QTextCharFormat linkFormat;
        linkFormat.setAnchor(true);
        linkFormat.setAnchorHref(tok->text);
        linkFormat.setForeground(OmniPainter::resolveColor(SemanticColor::LinkDefault));
        linkFormat.clearBackground();
        cursor.setCharFormat(linkFormat);
      }

      cursor.insertText(tok->text);
      cursor.setCharFormat(old);
    }
  }
};

class ScriptOutputRenderer : public QTextBrowser {
public:
  ScriptOutputRenderer() {
    setReadOnly(true);
    setVerticalScrollBar(new OmniScrollBar);
    setOpenExternalLinks(true);
  }

  void append(const QString &text) {
    m_output.append(text);
    renderDocument();
  }

  void clear() {
    m_output.clear();
    renderDocument();
  }

private:
  void renderDocument() {
    bool isBottomScrolled = verticalScrollBar()->value() == verticalScrollBar()->maximum();
    auto doc = new ScriptOutputTextDocument;
    QTextCursor cursor = textCursor();
    int selectionStart = cursor.selectionStart();
    int selectionEnd = cursor.selectionEnd();
    int scrollHeight = verticalScrollBar()->value();

    doc->setDocumentMargin(15);
    doc->setScriptText(m_output);
    setDocument(doc);
    verticalScrollBar()->setValue(scrollHeight);

    QTextCursor newCursor = textCursor();

    newCursor.setPosition(selectionStart);
    newCursor.setPosition(selectionEnd, QTextCursor::KeepAnchor);
    setTextCursor(newCursor);

    if (isBottomScrolled) {
      verticalScrollBar()->setValue(verticalScrollBar()->maximum());
    } else {
      verticalScrollBar()->setValue(scrollHeight);
    }

    if (m_doc) m_doc->deleteLater();
    m_doc = doc;
  }

  ScriptOutputTextDocument *m_doc = nullptr;
  QString m_output;
};
