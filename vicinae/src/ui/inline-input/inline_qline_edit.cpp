#include "ui/inline-input/inline_qline_edit.hpp"
#include "theme.hpp"
#include "theme/colors.hpp"
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
  int borderRadius = 5;
  OmniPainter painter(this);
  QPainterPath path;

  painter.setRenderHint(QPainter::Antialiasing, true);

  path.addRoundedRect(rect(), borderRadius, borderRadius);
  painter.setClipPath(path);
  painter.setThemeBrush(SemanticColor::LighterBackground);
  painter.drawPath(path);
  painter.setThemePen(m_error.isEmpty() ? SemanticColor::InputBorder : SemanticColor::InputBorderError);
  painter.drawPath(path);
  QLineEdit::paintEvent(event);
}
