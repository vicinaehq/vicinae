#include "command-database.hpp"
#include "extensions/browser/browser-extension.hpp"
#include "extensions/clipboard/clipboard-extension.hpp"
#include "extensions/calculator/calculator-extension.hpp"
#include "extensions/file/file-extension.hpp"
#include "extensions/internal/internal-extension.hpp"
#include "extensions/power-management/power-management-extension.hpp"
#include "extensions/shortcut/shortcut-extension.hpp"
#include "extensions/font/font-extension.hpp"
#include "extensions/snippet/snippet-extension.hpp"
#include "extensions/theme/theme-extension.hpp"
#include "extensions/developer/developer-extension.hpp"
#include "extensions/raycast/raycast-compat-extension.hpp"
#include "extensions/wm/wm-extension.hpp"
#include "extensions/vicinae/vicinae-extension.hpp"
#include "extensions/system/system-extension.hpp"
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

CommandDatabase::CommandDatabase() {
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
  registerRepository<WindowManagementExtension>();
  registerRepository<ThemeExtension>();
  registerRepository<FontExtension>();
  registerRepository<DeveloperExtension>();

#ifdef ENABLE_PREVIEW_FEATURES
  registerRepository<SnippetExtension>();
#endif

#ifdef QT_DEBUG
  registerRepository<InternalExtension>();
#endif

#ifdef Q_OS_UNIX
  registerRepository<SystemExtension>();
#endif
}
