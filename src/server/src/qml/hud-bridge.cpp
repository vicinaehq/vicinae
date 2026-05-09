#include "hud-bridge.hpp"
#include <QQuickWindow>

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

void HudBridge::registerWindow(QQuickWindow *window) { m_window = window; }
