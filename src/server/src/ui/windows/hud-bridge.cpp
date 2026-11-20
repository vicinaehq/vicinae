#include <QQuickWindow>
#include <utility>
#include "ui/windows/hud-bridge.hpp"

HudBridge::HudBridge(QObject *parent) : QObject(parent) {
  m_timer.setSingleShot(true);
  m_timer.setInterval(1500);
  connect(&m_timer, &QTimer::timeout, this, &HudBridge::hide);
}

void HudBridge::show(const QString &title, const std::optional<ImageURL> &icon) {
  if (m_dictation || m_activity) {
    m_pendingMessage = Message{title, icon};
    return;
  }
  m_text = title;
  m_icon = icon ? ImageUrl(*icon) : ImageUrl();
  emit contentChanged();

  showWindow();
  m_timer.start();
}

void HudBridge::hide() {
  if (!m_visible) return;
  m_visible = false;
  emit visibleChanged();

  if (m_window) { m_window->hide(); }
}

void HudBridge::showWindow() {
  m_visible = true;
  emit visibleChanged();

  if (m_window) {
    m_window->show();
    m_window->raise();
  }
}

void HudBridge::registerWindow(QQuickWindow *window) { m_window = window; }

void HudBridge::showDictation(DictationSession *session) {
  clearActivity();
  clearDictation();
  m_timer.stop();
  m_dictation = session;
  emit dictationChanged();

  connect(session, &DictationSession::finished, this, [this, session]() {
    if (m_dictation != session) return;
    clearDictation();
    hide();
    if (auto message = std::exchange(m_pendingMessage, std::nullopt)) { show(message->text, message->icon); }
  });

  showWindow();
}

void HudBridge::showActivity(HudActivity *activity) {
  clearDictation();
  clearActivity();
  m_timer.stop();
  m_activity = activity;
  emit activityChanged();

  connect(activity, &HudActivity::finished, this, [this, activity]() {
    if (m_activity != activity) return;
    clearActivity();
    hide();
    if (auto message = std::exchange(m_pendingMessage, std::nullopt)) { show(message->text, message->icon); }
  });

  showWindow();
}

void HudBridge::clearDictation() {
  if (!m_dictation) return;
  m_dictation->disconnect(this);
  m_dictation = nullptr;
  emit dictationChanged();
}

void HudBridge::clearActivity() {
  if (!m_activity) return;
  m_activity->disconnect(this);
  m_activity = nullptr;
  emit activityChanged();
}
