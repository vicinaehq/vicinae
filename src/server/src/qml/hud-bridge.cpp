#include "hud-bridge.hpp"
#include "environment.hpp"
#include "vicinae.hpp"
#include <QQuickWindow>
#ifdef WAYLAND_LAYER_SHELL
#include <LayerShellQt/Window>
#endif

HudBridge::HudBridge(QObject *parent) : QObject(parent) {
  m_timer.setSingleShot(true);
  m_timer.setInterval(1500);
  connect(&m_timer, &QTimer::timeout, this, &HudBridge::hide);
}

void HudBridge::show(const QString &title, const std::optional<ImageURL> &icon) {
  m_text = title;
  m_icon = icon ? ImageUrl(*icon) : ImageUrl();
  emit contentChanged();

  m_visible = true;
  emit visibleChanged();

  if (m_window) {
    m_window->show();
    m_window->raise();
  }

  m_timer.start();
}

void HudBridge::hide() {
  if (!m_visible) return;
  m_visible = false;
  emit visibleChanged();

  if (m_window) { m_window->hide(); }
}

void HudBridge::registerWindow(QQuickWindow *window) {
  m_window = window;
  if (m_window) { configureLayerShell(m_window); }
}

void HudBridge::configureLayerShell(QQuickWindow *window) {
#ifdef WAYLAND_LAYER_SHELL
  if (!Environment::isLayerShellSupported()) return;

  namespace Shell = LayerShellQt;
  if (auto *lshell = Shell::Window::get(window)) {
    lshell->setLayer(Shell::Window::LayerTop);
    lshell->setScope(QStringLiteral("vicinae-hud"));
    lshell->setAnchors(Shell::Window::AnchorNone);
    lshell->setKeyboardInteractivity(Shell::Window::KeyboardInteractivityNone);
  }
#else
  Q_UNUSED(window)
#endif
}
