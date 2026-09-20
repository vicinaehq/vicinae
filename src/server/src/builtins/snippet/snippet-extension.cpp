#include <algorithm>
#include "builtins/snippet/snippet-extension.hpp"
#include "services/snippet/snippet-service.hpp"
#include "service-registry.hpp"

void SnippetExtension::initialized(const SnippetPreferences &preferences) const {
  preferencesChanged(preferences);
}

void SnippetExtension::preferencesChanged(const SnippetPreferences &preferences) const {
  auto *snippet = ServiceRegistry::instance()->snippetService();
  snippet->setExpansionEnabled(preferences.enabled);
  snippet->setUndoEnabled(preferences.undo);

#ifdef Q_OS_LINUX
  snippet->setPrePasteDelay(std::clamp(preferences.prePasteDelay, 0, 5000));
  snippet->setKeyDelay(std::clamp(preferences.keyDelay, 0, 50) * 1000);
  snippet->setLayout(preferences.layout);
#endif
}
