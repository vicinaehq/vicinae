#include "ui/windows/hud-bridge.hpp"
#include <QQuickWindow>

HudBridge::HudBridge(QObject *parent) : QObject(parent) {
  m_timer.setSingleShot(true);
  m_timer.setInterval(1500);
  connect(&m_timer, &QTimer::timeout, this, &HudBridge::hide);
}

void HudBridge::show(const QString &title, const std::optional<ImageURL> &icon) {
  clearDictation();
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

void HudBridge::showDictation(DictationSession *session) {
  clearDictation();
  m_timer.stop();
  m_dictation = session;
  emit dictationChanged();

  connect(session, &DictationSession::finished, this, [this, session]() {
    if (m_dictation != session) return;
    clearDictation();
    hide();
  });

  m_visible = true;
  emit visibleChanged();

  if (m_window) {
    m_window->show();
    m_window->raise();
  }
}

void HudBridge::clearDictation() {
  if (!m_dictation) return;
  m_dictation->disconnect(this);
  m_dictation = nullptr;
  emit dictationChanged();
}
