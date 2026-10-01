#pragma once
#include <QMimeData>
#include <memory>
#include <optional>
#include "services/clipboard/clipboard-content.hpp"
#include "services/clipboard/clipboard-server.hpp"

namespace Clipboard {

std::optional<ClipboardSelection> selectionFromMimeData(const QMimeData *mimeData);
std::unique_ptr<QMimeData> mimeDataForContent(const Content &content);

} // namespace Clipboard
