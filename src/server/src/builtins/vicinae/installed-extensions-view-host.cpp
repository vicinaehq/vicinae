#include "builtins/vicinae/installed-extensions-view-host.hpp"
#include "builtins/vicinae/installed-extensions-model.hpp"
#include "service-registry.hpp"
#include "services/extension-registry/extension-registry.hpp"

void InstalledExtensionsViewHost::initialize() {
  BaseView::initialize();
  initModel();

  model()->addSource(&m_section);

  setSearchPlaceholderText(tr("Search extensions..."));

  auto registry = ServiceRegistry::instance()->extensionRegistry();
  connect(registry, &ExtensionRegistry::extensionsChanged, this, &InstalledExtensionsViewHost::reload);
}

void InstalledExtensionsViewHost::loadInitialData() { reload(); }

void InstalledExtensionsViewHost::reload() {
  auto registry = ServiceRegistry::instance()->extensionRegistry();
  m_section.setItems(registry->scanAll());
}
