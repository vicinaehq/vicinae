#include "extension/extension-background-runner.hpp"
#include "extension/extension-command-runtime.hpp"
#include "extension/manager/extension-manager.hpp"
#include "service-registry.hpp"
#include <algorithm>

ExtensionBackgroundRunner::ExtensionBackgroundRunner(ApplicationContext &context) : m_context(context) {
  auto *manager = context.services->extensionManager();
  QObject::connect(manager, &ExtensionManager::extensionUnloaded, this, [this](const std::string &sessionId) {
    if (auto it = m_runtimes.find(sessionId); it != m_runtimes.end()) complete(it->second);
  });
  QObject::connect(manager, &ExtensionManager::extensionCrashed, this,
                   [this](const std::string &sessionId, const std::string &) {
                     if (auto it = m_runtimes.find(sessionId); it != m_runtimes.end()) complete(it->second);
                   });
}

ExtensionBackgroundRunner::~ExtensionBackgroundRunner() {
  while (!m_runtimes.empty()) {
    auto *runtime = m_runtimes.begin()->second;
    m_runtimes.erase(m_runtimes.begin());
    runtime->unload();
    delete runtime;
  }
}

bool ExtensionBackgroundRunner::launch(const std::shared_ptr<ExtensionCommand> &command, const LaunchProps &props) {
  if (!command->isNoView() || !m_context.services->extensionManager()->isRunning()) return false;

  auto *runtime = new ExtensionCommandRuntime(command);
  runtime->setHeadless(true);
  runtime->setContext(&m_context);

  QObject::connect(runtime, &ExtensionCommandRuntime::loaded, this, [this, runtime](const std::string &sessionId) {
    m_runtimes.emplace(sessionId, runtime);
  });
  QObject::connect(runtime, &ExtensionCommandRuntime::loadFailed, this, [runtime](const QString &) {
    runtime->complete();
    runtime->deleteLater();
  });

  runtime->load(props);
  return true;
}

void ExtensionBackgroundRunner::complete(ExtensionCommandRuntime *runtime) {
  std::erase_if(m_runtimes, [runtime](const auto &entry) { return entry.second == runtime; });
  runtime->complete();
  runtime->deleteLater();
}
