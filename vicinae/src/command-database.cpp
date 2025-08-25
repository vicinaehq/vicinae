#include "command-database.hpp"
#include "extensions/clipboard/clipboard-extension.hpp"
#include "extensions/calculator/calculator-extension.hpp"
#include "extensions/file/file-extension.hpp"
#include "extensions/shortcut/shortcut-extension.hpp"
#include "extensions/font/font-extension.hpp"
#include "extensions/theme/theme-extension.hpp"
#include "extensions/developer/developer-extension.hpp"
#include "extensions/raycast/raycast-compat-extension.hpp"
#include "extensions/wm/wm-extension.hpp"
#include "extensions/vicinae/vicinae-extension.hpp"
#include <memory>

const AbstractCmd *CommandDatabase::findCommand(const QString &id) {
  if (auto repo = findRepository(id)) {
    for (const auto &cmd : repo->commands()) {
      if (cmd->uniqueId() == id) { return cmd.get(); }
    }
  }

  return nullptr;
}

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
  registerRepository<RaycastCompatExtension>();
  registerRepository<VicinaeExtension>();
  registerRepository<CalculatorExtension>();
  registerRepository<ShortcutExtension>();
  registerRepository<WindowManagementExtension>();
  registerRepository<ThemeExtension>();
  registerRepository<FontExtension>();
  registerRepository<DeveloperExtension>();
}
