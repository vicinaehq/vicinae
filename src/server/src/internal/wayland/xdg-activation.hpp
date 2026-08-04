#pragma once

#include <QString>
#include <cstdint>
#include <optional>

class QWindow;

namespace Wayland::XdgActivation {

bool isSupported();

/**
 * Token to pass to a launched app through XDG_ACTIVATION_TOKEN, allowing it to take focus.
 * Uses the currently focused window and last input serial as provenance.
 */
std::optional<QString> requestLaunchToken(const QString &appId);

/**
 * Record the input serial of a compositor-forwarded trigger (e.g. a vicinae-hotkey press) so the
 * next activateWindow() call can use it. Qt's own last input serial is stale in that situation
 * because the compositor consumed the triggering key event.
 */
void setPendingSerial(std::uint32_t serial);

/** Focus one of our own windows, consuming the pending trigger serial if any. */
bool activateWindow(QWindow *window);

} // namespace Wayland::XdgActivation
