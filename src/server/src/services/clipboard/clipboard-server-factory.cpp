#include "services/clipboard/clipboard-server-factory.hpp"
#include "services/clipboard/clipboard-server.hpp"
#include "dummy/dummy-clipboard-server.hpp"

std::unique_ptr<AbstractClipboardServer> ClipboardServerFactory::createFirstActivatable() const {
  std::vector<std::unique_ptr<AbstractClipboardServer>> activatable;

  for (const auto &factory : m_registeredServers) {
    if (auto server = factory(); server && server->isActivatable()) {
      activatable.emplace_back(std::move(server));
    }
  }

  if (activatable.empty()) { return std::make_unique<DummyClipboardServer>(); }

  auto cmp = [](const auto &a, const auto &b) { return a->activationPriority() > b->activationPriority(); };

  std::ranges::sort(activatable, cmp);

  return std::move(activatable.front());
}
