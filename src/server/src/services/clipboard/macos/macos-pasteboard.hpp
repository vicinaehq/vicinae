#pragma once

#include <AppKit/AppKit.h>
#include "services/clipboard/clipboard-server.hpp"

namespace MacosClipboard {

bool writePasteboard(NSPasteboard *pasteboard, QMimeData *data, const Clipboard::CopyOptions &options);

} // namespace MacosClipboard
