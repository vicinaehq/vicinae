#include "ui/switch/switch.hpp"
#include "theme.hpp"
#include <qjsonvalue.h>
#include <qnamespace.h>

void Switch::paintEvent(QPaintEvent *event) {
  auto &theme = ThemeService::instance().theme();
  OmniPainter painter(this);
  int radius = height() / 2;

  if (m_value) {
    painter.setThemeBrush(SemanticColor::Blue);
  } else {
    painter.setThemeBrush(SemanticColor::MainHoverBackground);
  }

  painter.setRenderHint(QPainter::Antialiasing);
  painter.setPen(Qt::NoPen);
  painter.drawRoundedRect(rect(), radius, radius);

  if (m_value) {
    painter.setThemeBrush(SemanticColor::TextOnAccent);
  } else {
    painter.setThemeBrush(SemanticColor::TextSecondary);
  }

  QRect geometry;
  int padding = 2;
  int availableHeight = height() - padding * 2;

  if (m_value) {
    int xpos = width() - padding - availableHeight;
    int ypos = padding;

    geometry = QRect{xpos, ypos, availableHeight, availableHeight};
  } else {
    int xpos = padding;
    int ypos = padding;

    geometry = QRect{xpos, ypos, availableHeight, availableHeight};
  }

  painter.setPen(Qt::NoPen);
  painter.drawEllipse(geometry);
}

bool Switch::event(QEvent *event) {
  switch (event->type()) {
  case QEvent::FocusIn:
    m_focusNotifier->focusChanged(true);
    break;
  case QEvent::FocusOut:
    m_focusNotifier->focusChanged(false);
    break;
  default:
    break;
  }

  return QWidget::event(event);
};

void Switch::mousePressEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    toggle();
    return;
  }

  QWidget::mousePressEvent(event);
}

void Switch::resizeEvent(QResizeEvent *event) {
  QWidget::resizeEvent(event);
  setMaximumWidth(height() * 2);
}

FocusNotifier *Switch::focusNotifier() const { return m_focusNotifier; }

QSize Switch::sizeHint() const { return {50, 22}; }

void Switch::toggle() { setValue(!m_value); }
bool Switch::value() const { return m_value; }
void Switch::setValue(bool value) {
  m_value = value;
  update();
}
QJsonValue Switch::asJsonValue() const { return value(); }
void Switch::setValueAsJson(const QJsonValue &value) { setValue(value.toBool()); }

Switch::Switch() { setFocusPolicy(Qt::StrongFocus); }
