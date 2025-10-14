#include "ui/inline-input/inline_qline_edit.hpp"
#include <QLineEdit>
#include <QStyle>
#include <qpainter.h>
#include <qpainterpath.h>

InlineQLineEdit::InlineQLineEdit(const QString &placeholder, QWidget *parent) : QLineEdit(parent) {
  connect(this, &QLineEdit::textChanged, this, &InlineQLineEdit::handleTextChanged);

  setPlaceholderText(placeholder);
  resizeFromText(placeholder + "...");
  setTextMargins(5, 5, 0, 5);
  setProperty("arg-form-input", true);
}

void InlineQLineEdit::resizeFromText(const QString &s) {
  auto fm = fontMetrics();

  setFixedWidth(fm.boundingRect(s).width() + 15);
}

void InlineQLineEdit::handleTextChanged(const QString &s) {
  const QString &text = s.isEmpty() ? placeholderText() : s;

  resizeFromText(text);
}

void InlineQLineEdit::clearError() { setError(""); }

void InlineQLineEdit::setError(const QString &error) {
  m_error = error;
  setProperty("arg-form-input", error.isEmpty());
  setProperty("error", !error.isEmpty());
  style()->unpolish(this);
  style()->polish(this);
  update();
}
