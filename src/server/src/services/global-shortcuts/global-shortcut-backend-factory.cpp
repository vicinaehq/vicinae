#include "services/global-shortcuts/global-shortcut-backend-factory.hpp"
#include "services/global-shortcuts/dummy-global-shortcut-backend.hpp"

#ifdef Q_OS_LINUX
#include "services/global-shortcuts/input-server-global-shortcut-backend.hpp"
#include "services/global-shortcuts/global-shortcut-portal-backend.hpp"
#include <algorithm>
#include <vector>

std::unique_ptr<AbstractGlobalShortcutBackend> createGlobalShortcutBackend(LinuxInputServer *inputServer) {
  std::vector<std::unique_ptr<AbstractGlobalShortcutBackend>> candidates;
  candidates.reserve(3);

  candidates.emplace_back(std::make_unique<GlobalShortcutPortalBackend>());

  if (inputServer) {
    candidates.emplace_back(std::make_unique<InputServerGlobalShortcutBackend>(*inputServer));
  }

  std::erase_if(candidates, [](const auto &backend) { return !backend->isActivatable(); });

  if (candidates.empty()) { return std::make_unique<DummyGlobalShortcutBackend>(); }

  std::ranges::sort(candidates, [](const auto &a, const auto &b) {
    return a->activationPriority() > b->activationPriority();
  });

  return std::move(candidates.front());
}
#else
std::unique_ptr<AbstractGlobalShortcutBackend> createGlobalShortcutBackend() {
  return std::make_unique<DummyGlobalShortcutBackend>();
}
#endif
