#pragma once
#include <optional>
#include <QMimeData>
#include "services/clipboard/clipboard-server.hpp"

namespace Clipboard {

std::optional<ClipboardSelection> selectionFromMimeData(const QMimeData *mimeData);

}
