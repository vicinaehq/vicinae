#include "settings-controller/settings-controller.hpp"

void SettingsController::openWindow() const { emit windowVisiblityChangeRequested(true); }
void SettingsController::closeWindow() const { emit windowVisiblityChangeRequested(false); }
void SettingsController::openTab(const QString &tabId) { emit tabIdOpened(tabId); }

void SettingsController::openExtensionPreferences(const QString &id) {
  openTab("extensions");
  emit openExtensionPreferencesRequested(id);
}

SettingsController::SettingsController() {}
