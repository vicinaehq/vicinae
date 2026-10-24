#pragma once
#include <QMimeData>
#include <memory>
#include <optional>
#include "services/clipboard/clipboard-content.hpp"
#include "services/clipboard/clipboard-server.hpp"

namespace Clipboard {

std::optional<ClipboardSelection> selectionFromMimeData(const QMimeData *mimeData);
std::unique_ptr<QMimeData> mimeDataForContent(const Content &content);
std::unique_ptr<QMimeData> mimeDataForSelection(const ClipboardSelection &selection);

/**
 * The most preferred non-empty plain text offer, or `selection.offers.end()` if there is none.
 */
std::vector<ClipboardDataOffer>::const_iterator findPlainTextOffer(const ClipboardSelection &selection);

} // namespace Clipboard
