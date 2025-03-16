#include "clipboard/clipboard-server.hpp"
#include <qobject.h>

class ClipboardServerFactory {
public:
  AbstractClipboardServer *createFirstActivatable(QObject *parent = nullptr) const;
  AbstractClipboardServer *create(ClipboardServerType type, QObject *parent = nullptr) const;
};
