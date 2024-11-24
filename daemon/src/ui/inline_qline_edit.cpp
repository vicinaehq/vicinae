#include "ui/inline_qline_edit.hpp"
#include <QLineEdit>

InlineQLineEdit::InlineQLineEdit(const QString &placeholder, QWidget *parent)
    : QLineEdit(parent) {
  connect(this, &QLineEdit::textChanged, this, &InlineQLineEdit::textChanged);

  setPlaceholderText(placeholder);
  resizeFromText(placeholder + "...");
  setTextMargins(5, 5, 0, 5);
}

void InlineQLineEdit::resizeFromText(const QString &s) {
  auto fm = fontMetrics();

  setFixedWidth(fm.boundingRect(s).width() + 15);
}

void InlineQLineEdit::textChanged(const QString &s) {
  const QString &text = s.isEmpty() ? placeholderText() : s;

  resizeFromText(text);
}
