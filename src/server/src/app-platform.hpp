#pragma once

namespace AppPlatform {

// Before QGuiApplication: Qt's Cocoa input context initialises TSM, which must happen in a UI app
// or the panel never gets typing focus (Caps Lock layout switching).
void beforeGuiApplication();
void afterGuiApplication();

} // namespace AppPlatform
