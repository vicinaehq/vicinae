#include "ui/spinner/spinner.hpp"
#include "theme.hpp"
#include "theme/colors.hpp"
#include "ui/omni-painter/omni-painter.hpp"

LoadingSpinner::LoadingSpinner(QWidget *parent)
    : QWidget(parent), m_size(40), m_thickness(2), m_startAngle(0), m_color(QColor(0, 122, 255)) {
  m_timer = new QTimer(this);
  connect(m_timer, &QTimer::timeout, this, &LoadingSpinner::rotate);

  setFixedSize(m_size + m_thickness * 2, m_size + m_thickness * 2);
}

void LoadingSpinner::start() {
  m_timer->start(1000 / 60);
  show();
}

void LoadingSpinner::stop() {
  m_timer->stop();
  hide();
}

void LoadingSpinner::setColor(const ColorLike &color) {
  m_color = color;
  update();
}

void LoadingSpinner::setSize(int size) {
  m_size = size;
  setFixedSize(m_size + m_thickness * 2, m_size + m_thickness * 2);
  update();
}

void LoadingSpinner::setThickness(int thickness) {
  m_thickness = thickness;
  setFixedSize(m_size + m_thickness * 2, m_size + m_thickness * 2);
  update();
}

void LoadingSpinner::paintEvent(QPaintEvent *event) {
  Q_UNUSED(event);

  OmniPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  int side = qMin(width(), height());

  painter.translate(width() / 2.0, height() / 2.0);

  QColor color = painter.resolveColor(SemanticColor::DynamicToastSpinner);
  QPen pen(color, m_thickness, Qt::SolidLine, Qt::RoundCap);
  painter.setPen(pen);
  painter.setBrush(Qt::NoBrush);

  QRect rect(-m_size / 2, -m_size / 2, m_size, m_size);

  // Draw arc (270 degrees span)
  painter.drawArc(rect, m_startAngle * 16, 270 * 16);
}

void LoadingSpinner::rotate() {
  m_startAngle += 6;
  if (m_startAngle >= 360) m_startAngle -= 360;
  update();
}
