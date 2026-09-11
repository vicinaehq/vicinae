#include "command/command-database.hpp"
#include "builtins/browser/browser-extension.hpp"
#include "builtins/clipboard/clipboard-extension.hpp"
#include "builtins/calculator/calculator-extension.hpp"
#include "builtins/file/file-extension.hpp"
#include "builtins/internal/internal-extension.hpp"
#include "builtins/media/media-extension.hpp"
#include "builtins/power-management/power-management-extension.hpp"
#include "builtins/shortcut/shortcut-extension.hpp"
#include "builtins/font/font-extension.hpp"
#include "builtins/snippet/snippet-extension.hpp"
#include "builtins/theme/theme-extension.hpp"
#include "builtins/developer/developer-extension.hpp"
#include "builtins/raycast/raycast-compat-extension.hpp"
#include "builtins/wm/wm-extension.hpp"
#include "builtins/vicinae/vicinae-extension.hpp"
#include "builtins/system/system-extension.hpp"
#include "builtins/ai/intelligence-extension.hpp"
#include "builtins/dictation/dictation-extension.hpp"
#include "service-registry.hpp"
#include <memory>

const std::vector<std::shared_ptr<AbstractCommandRepository>> &CommandDatabase::repositories() const {
  return _repositories;
}

const AbstractCommandRepository *CommandDatabase::findRepository(const QString &id) {
  for (const auto &repository : repositories()) {
    if (repository->id() == id) return repository.get();
  }

  return nullptr;
}

CommandDatabase::CommandDatabase(const ServiceRegistry &services) {
  registerRepository<ClipboardExtension>();
  registerRepository<FileExtension>();
  registerRepository<PowerManagementExtension>();
  registerRepository<BrowserExtension>();

#ifdef HAS_TYPESCRIPT_EXTENSIONS
  registerRepository<RaycastCompatExtension>();
#endif
  registerRepository<VicinaeExtension>();
  registerRepository<CalculatorExtension>();
  registerRepository<ShortcutExtension>();

  registerRepository<WindowManagementExtension>(services);

  registerRepository<ThemeExtension>();
  registerRepository<FontExtension>();
  registerRepository<DeveloperExtension>();

  registerRepository<SnippetExtension>();

  registerRepository<DictationExtension>();
  registerRepository<IntelligenceExtension>();

#ifdef QT_DEBUG
  registerRepository<InternalExtension>();
#endif

  registerRepository<MediaExtension>();

#ifdef Q_OS_UNIX
  registerRepository<SystemExtension>();
#endif
}
