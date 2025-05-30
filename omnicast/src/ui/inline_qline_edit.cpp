#include "ui/inline_qline_edit.hpp"
#include "theme.hpp"
#include <QLineEdit>
#include <qpainter.h>
#include <qpainterpath.h>

InlineQLineEdit::InlineQLineEdit(const QString &placeholder, QWidget *parent) : QLineEdit(parent) {
  connect(this, &QLineEdit::textChanged, this, &InlineQLineEdit::handleTextChanged);

  setPlaceholderText(placeholder);
  resizeFromText(placeholder + "...");
  setTextMargins(5, 5, 0, 5);
}

void InlineQLineEdit::resizeFromText(const QString &s) {
  auto fm = fontMetrics();

  setFixedWidth(fm.boundingRect(s).width() + 15);
}

void InlineQLineEdit::handleTextChanged(const QString &s) {
  const QString &text = s.isEmpty() ? placeholderText() : s;

  resizeFromText(text);
}

void InlineQLineEdit::paintEvent(QPaintEvent *event) {
  auto &theme = ThemeService::instance().theme();
  int borderRadius = 5;

  QPainter painter(this);

  painter.setRenderHint(QPainter::Antialiasing, true);

  QPainterPath path;
  path.addRoundedRect(rect(), borderRadius, borderRadius);

  painter.setClipPath(path);

  painter.fillPath(path, theme.colors.statusBackground);

  // Draw the border
  QPen pen(theme.colors.statusBackgroundBorder, 1); // Border with a thickness of 2
  painter.setPen(pen);
  painter.drawPath(path);

  QLineEdit::paintEvent(event);
}
