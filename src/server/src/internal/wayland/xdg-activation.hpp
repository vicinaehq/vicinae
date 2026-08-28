#pragma once

#include <QString>
#include <cstdint>
#include <optional>

class QWindow;

namespace Wayland::XdgActivation {
bool isSupported();
std::optional<QString> requestLaunchToken(const QString &appId);
void setPendingSerial(std::uint32_t serial);
bool activateWindow(QWindow *window);
} // namespace Wayland::XdgActivation
