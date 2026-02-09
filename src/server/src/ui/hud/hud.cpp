#include "hud.hpp"
#include "service-registry.hpp"
#include "ui/omni-painter/omni-painter.hpp"
#include "utils/environment.hpp"
#include "utils/layout.hpp"
#include "config/config.hpp"
#include "LayerShellQt/Window"
#include "vicinae.hpp"

void HudWidget::paintEvent(QPaintEvent *event) {
  const auto &cfg = ServiceRegistry::instance()->config()->value();
  QPainter painter(this);
  int radius = m_shouldDrawBorders ? (height() / 2) : 0;
  auto color = OmniPainter::resolveColor(SemanticColor::Background);

  color.setAlphaF(cfg.launcherWindow.opacity);
  // helps getting rid of weird opacity artifacting on first launch
  painter.setCompositionMode(QPainter::CompositionMode_Source);
  painter.fillRect(rect(), Qt::transparent);
  painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

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
  if (Environment::isLayerShellSupported()) {
    namespace Shell = LayerShellQt;

    createWinId();
    if (auto lshell = Shell::Window::get(windowHandle())) {
      lshell->setLayer(Shell::Window::LayerTop);
      lshell->setScope(Omnicast::APP_ID);
      lshell->setScreenConfiguration(Shell::Window::ScreenFromCompositor);
      lshell->setKeyboardInteractivity(Shell::Window::KeyboardInteractivityNone);
      lshell->setAnchors(Shell::Window::AnchorNone);
    } else {
      qWarning() << "Unable apply layer shell rules to hud window: LayerShellQt::Window::get() returned null";
    }
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
