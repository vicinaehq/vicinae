#include "settings-controller/settings-controller.hpp"
#include "common.hpp"
#include "qml/settings-window.hpp"
#include <QTimer>

SettingsController::SettingsController(ApplicationContext &ctx) : m_ctx(ctx) {}

SettingsController::~SettingsController() {
  if (m_window) m_window->deleteLater();
}

void SettingsController::openWindow() {
  closeWindow(false);
  createSettingsWindow();
  m_window->show();
}

bool SettingsController::closeWindow(bool destroy) {
  if (!m_window || m_isClosing) return false;
  m_isClosing = true;
  m_window->hide();
  if (destroy) { destroySettingsWindow(); }
  m_isClosing = false;
  return true;
}

void SettingsController::openTab(const QString &tabId) {
  openWindow();
  QTimer::singleShot(0, [this, tabId]() {
    if (m_window) m_window->openTab(tabId);
  });
}

void SettingsController::openExtensionPreferences(const EntrypointId &id) {
  openTab("extensions");
  QTimer::singleShot(0, [this, id]() {
    if (m_window) m_window->selectExtension(QString::fromStdString(id));
  });
}

void SettingsController::createSettingsWindow() {
  if (m_window) return;
  m_window = new SettingsWindow(m_ctx);
}

void SettingsController::destroySettingsWindow() {
  if (!m_window) return;
  m_window->deleteLater();
  m_window = nullptr;
}
