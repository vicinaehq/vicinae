#pragma once
#include <cstdint>
#include <optional>
#include <qnamespace.h>

namespace global_shortcuts {

/**
 * Maps a Qt::Key to its base-layer xkb keysym (XKB_KEY_*), or nullopt if unsupported.
 * Shared by the input-server and portal backends.
 */
std::optional<uint32_t> xkbKeysymForQtKey(Qt::Key key);

} // namespace global_shortcuts
