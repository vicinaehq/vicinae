#include "settings-controller/settings-controller.hpp"
#include "settings/settings-window.hpp"

void SettingsController::openWindow() {
  // if window is already shown, hide/show again to force it to reposition itself
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
  emit tabIdOpened(tabId);
}

void SettingsController::openExtensionPreferences(const QString &id) {
  openTab("extensions");
  emit openExtensionPreferencesRequested(id);
}

SettingsController::SettingsController(ApplicationContext &ctx) : m_ctx(ctx) {}

void SettingsController::createSettingsWindow() {
  if (m_window) return;
  m_window = new SettingsWindow(&m_ctx);
}

void SettingsController::destroySettingsWindow() {
  if (!m_window) return;
  m_window->deleteLater();
  m_window = nullptr;
}
