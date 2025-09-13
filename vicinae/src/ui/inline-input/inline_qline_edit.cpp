#include "ui/inline-input/inline_qline_edit.hpp"
#include "theme.hpp"
#include "ui/omni-painter/omni-painter.hpp"
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

void InlineQLineEdit::clearError() { setError(""); }

void InlineQLineEdit::setError(const QString &error) {
  m_error = error;
  update();
}

void InlineQLineEdit::paintEvent(QPaintEvent *event) {
  auto &theme = ThemeService::instance().theme();
  int borderRadius = 5;
  ColorLike borderColor = m_error.isEmpty() ? theme.resolveTint(SemanticColor::StatusBorder)
                                            : theme.resolveTint(SemanticColor::Red);
  OmniPainter painter(this);
  QBrush borderBrush = painter.colorBrush(borderColor);

  painter.setRenderHint(QPainter::Antialiasing, true);

  QPainterPath path;
  path.addRoundedRect(rect(), borderRadius, borderRadius);

  painter.setClipPath(path);

  painter.fillPath(path, theme.resolveTint(SemanticColor::StatusBackground));

  QPen pen(borderBrush, 1);
  painter.setPen(pen);
  painter.drawPath(path);

  QLineEdit::paintEvent(event);
}
