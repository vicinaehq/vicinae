#include "hud.hpp"
#include "utils/environment.hpp"
#include "utils/layout.hpp"
#include <qnamespace.h>
#include <qtenvironmentvariables.h>
#include "LayerShellQt/Window"
#include "vicinae.hpp"

void HudWidget::paintEvent(QPaintEvent *event) {
  OmniPainter painter(this);
  int radius = m_shouldDrawBorders ? (height() / 2) : 0;
  auto color = painter.resolveColor(SemanticColor::MainBackground);
  painter.setRenderHint(QPainter::Antialiasing);
  painter.setBrush(color);
  painter.setPen(Qt::NoPen);
  painter.drawRoundedRect(rect(), radius, radius);
  QWidget::paintEvent(event);
}

void HudWidget::setupUI() {
  setWindowFlags(Qt::FramelessWindowHint);
  setAttribute(Qt::WA_TranslucentBackground, true);

#ifdef WAYLAND_LAYER_SHELL
  if (Environment::isLayerShellEnabled()) {
    namespace Shell = LayerShellQt;

    createWinId();
    if (auto lshell = Shell::Window::get(windowHandle())) {
      lshell->setLayer(Shell::Window::LayerOverlay);
      lshell->setScope(Omnicast::APP_ID);
      lshell->setScreenConfiguration(Shell::Window::ScreenFromCompositor);
      lshell->setKeyboardInteractivity(Shell::Window::KeyboardInteractivityNone);
      lshell->setExclusiveZone(-1);
      lshell->setAnchors(Shell::Window::AnchorNone);
    } else {
      qWarning() << "Unable apply layer shell rules to hud window: LayerShellQt::Window::get() returned null";
    }
  } else {
    qInfo() << "USE_LAYER_SHELL=0 is set, not using layer shell";
  }
#endif

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
