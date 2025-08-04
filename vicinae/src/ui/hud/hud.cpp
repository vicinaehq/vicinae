#include "hud.hpp"
#include "utils/layout.hpp"

void HudWidget::paintEvent(QPaintEvent *event) {
  OmniPainter painter(this);
  int radius = m_shouldDrawBorders ? (height() / 2) : 0;
  auto color = painter.resolveColor(SemanticColor::MainBackground);

  color.setAlphaF(0.6);
  painter.setRenderHint(QPainter::Antialiasing);
  painter.setBrush(color);
  painter.setPen(Qt::NoPen);
  painter.drawRoundedRect(rect(), radius, radius);
  QWidget::paintEvent(event);
}

void HudWidget::setupUI() {
  setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
  setAttribute(Qt::WA_TranslucentBackground, true);
  auto content = HStack().add(UI::Icon().size({16, 16}).ref(m_icon)).add(UI::Text("").ref(m_text)).spacing(5);

  VStack().add(content.buildWidget(), 0, Qt::AlignCenter).margins(15, 10, 15, 10).imbue(this);
  clear();
}

void HudWidget::setIcon(const ImageURL &icon) {
  m_icon->setUrl(icon);
  m_icon->show();
}

void HudWidget::setText(const QString &text) { m_text->setText(text); }

void HudWidget::setClientSideBorderDrawing(bool value) { m_shouldDrawBorders = value; }

void HudWidget::clear() {
  m_text->clear();
  m_icon->hide();
}
