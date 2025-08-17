#include "services/clipboard/clipboard-server.hpp"
#include <qobject.h>

template <typename T>
concept ClipboardServer = std::derived_from<T, AbstractClipboardServer>;

class ClipboardServerFactory {
  using Factory = std::function<std::unique_ptr<AbstractClipboardServer>()>;
  std::vector<Factory> m_registeredServers;

public:
  std::unique_ptr<AbstractClipboardServer> createFirstActivatable() const;

  template <ClipboardServer T> void registerServer() {
    m_registeredServers.emplace_back([]() { return std::make_unique<T>(); });
  }
};
