#include "text-area.hpp"
#include "common.hpp"
#include "theme.hpp"
#include "ui/scroll-bar/scroll-bar.hpp"
#include "utils/layout.hpp"
#include <QPlainTextEdit>
#include <qevent.h>
#include <qnamespace.h>
#include <qplaintextedit.h>
#include <qwidget.h>

int TextArea::heightForRowCount(int rowCount) {
  int margin = m_textEdit->document()->documentMargin();

  return margin * 2 + (rowCount * m_textEdit->fontMetrics().lineSpacing());
}

void TextArea::setRows(size_t rowCount) {
  m_rows = rowCount;
  setFixedHeight(heightForRowCount(rowCount));
}

void TextArea::resizeEvent(QResizeEvent *event) {
  JsonFormItemWidget::resizeEvent(event);
  resizeArea();
}

void TextArea::setupUI() {
  m_textEdit = new QPlainTextEdit;
  m_textEdit->setFrameShape(QFrame::NoFrame);
  m_textEdit->setVerticalScrollBar(new OmniScrollBar);
  m_notifier->track(m_textEdit);
  setFocusProxy(m_textEdit);
  setGrowAsRequired(true);
  setTabSetFocus(true);
  setMargins(10);
  setRows(2);
  VStack().add(m_textEdit).imbue(this);

  connect(m_textEdit, &QPlainTextEdit::textChanged, this, &TextArea::resizeArea);
}

void TextArea::setMargins(int margins) { m_textEdit->document()->setDocumentMargin(margins); }

void TextArea::resizeArea() {
  if (m_growAsRequired) {
    QFontMetrics fm = m_textEdit->fontMetrics();
    int minTextHeight = heightForRowCount(m_rows);
    // document height is number of rows in QPlainTextDocument
    int rowCount = m_textEdit->document()->size().height();
    int textHeight = heightForRowCount(rowCount);
    int height = std::max(minTextHeight, textHeight);

    setFixedHeight(height);
    m_textEdit->setFixedHeight(height);
  }
}

void TextArea::paintEvent(QPaintEvent *event) {
  int borderRadius = 6;
  OmniPainter painter(this);

  painter.setThemePen(hasFocus() ? SemanticColor::InputBorderFocus : SemanticColor::InputBorder, 3);
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.drawRoundedRect(rect(), borderRadius, borderRadius);

  JsonFormItemWidget::paintEvent(event);
}

QJsonValue TextArea::asJsonValue() const { return m_textEdit->toPlainText(); };

void TextArea::setValueAsJson(const QJsonValue &value) { m_textEdit->setPlainText(value.toString()); };

void TextArea::setTabSetFocus(bool ignore) { m_textEdit->setTabChangesFocus(ignore); }

void TextArea::setGrowAsRequired(bool value) {
  if (value) {
    m_textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
  } else {
    m_textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
  }
  m_growAsRequired = true;
}

void TextArea::setText(const QString &text) { m_textEdit->setPlainText(text); }

void TextArea::setPlaceholderText(const QString &text) { m_textEdit->setPlaceholderText(text); }

QString TextArea::text() const { return m_textEdit->toPlainText(); }

TextArea::TextArea(QWidget *parent) : JsonFormItemWidget(parent) { setupUI(); }
