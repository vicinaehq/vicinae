#include "services/clipboard/clipboard-server.hpp"
#include "utils.hpp"
#include <qdebug.h>

QDebug operator<<(QDebug debug, const ClipboardSelection &selection) {
  QDebugStateSaver saver(debug);

  debug.nospace() << "ClipboardSelection(" << selection.offers.size() << ") {\n";

  for (const auto &offer : selection.offers) {
    debug << "    Offer(" << offer.mimeType << ") { "
          << "size = " << formatSize(offer.data.size()) << " }\n";
  }

  debug << "}";

  return debug;
}
