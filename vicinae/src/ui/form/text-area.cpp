#include "text-area.hpp"
#include "common.hpp"
#include "theme.hpp"
#include "ui/scroll-bar/scroll-bar.hpp"
#include "utils/layout.hpp"
#include <QPlainTextEdit>
#include <qapplication.h>
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

void TextArea::setForwardShiftReturn(bool value) { m_forwardShiftReturn = value; }

bool TextArea::forwardShiftReturn() const { return m_forwardShiftReturn; }

void TextArea::setupUI() {
  m_textEdit = new QPlainTextEdit;
  m_textEdit->setFrameShape(QFrame::NoFrame);
  m_textEdit->setVerticalScrollBar(new OmniScrollBar);
  m_textEdit->setProperty("form-input", true);
  m_textEdit->installEventFilter(this);
  m_notifier->track(m_textEdit);
  setFocusProxy(m_textEdit);
  setGrowAsRequired(true);
  setTabSetFocus(true);
  setMargins(10);
  setRows(2);
  VStack().add(m_textEdit).imbue(this);

  setStyleSheet(ThemeService::instance().inputStyleSheet());
  connect(&ThemeService::instance(), &ThemeService::themeChanged, this,
          [this]() { setStyleSheet(ThemeService::instance().inputStyleSheet()); });
  connect(m_textEdit, &QPlainTextEdit::textChanged, this, &TextArea::resizeArea);
}

QPlainTextEdit *TextArea::textEdit() const { return m_textEdit; }

bool TextArea::eventFilter(QObject *watched, QEvent *event) {
  if (watched == m_textEdit && event->type() == QEvent::KeyPress) {
    auto keyEvent = static_cast<QKeyEvent *>(event);
    bool hasShift = keyEvent->modifiers().testFlag(Qt::KeyboardModifier::ShiftModifier);

    if (hasShift && m_forwardShiftReturn &&
        (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter)) {
      QApplication::sendEvent(parentWidget(), keyEvent);
      return true;
    }
  }

  return QWidget::eventFilter(watched, event);
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
