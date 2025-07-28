#include "settings-controller/settings-controller.hpp"

void SettingsController::openWindow() const { emit windowVisiblityChangeRequested(true); }
void SettingsController::closeWindow() const { emit windowVisiblityChangeRequested(false); }

SettingsController::SettingsController() {}
