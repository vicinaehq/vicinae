#pragma once
#include "actions/files/file-actions.hpp"
#include "clipboard-actions.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/app-service/app-service.hpp"
#include "services/toast/toast-service.hpp"
#include "utils/file-browser-utils.hpp"
#include <qmimedatabase.h>
#include <filesystem>
#include <memory>

namespace FileActions {

class RevealFileInFolderAction : public AbstractAction {
public:
  RevealFileInFolderAction(std::filesystem::path path)
      : AbstractAction("Open in folder", ImageURL::builtin("folder")), m_path(std::move(path)) {
    setShortcut(Keyboard::Shortcut::submit());
  }

  void execute(ApplicationContext *ctx) override {
    auto const appDb = ctx->services->appDb();
    auto const files = ctx->services->fileService();
    auto const toast = ctx->services->toastService();

    bool const success = FileBrowser::showInFileBrowser(m_path, appDb, true);

    if (!success) {
      toast->failure("Failed to open folder");
      return;
    }

    files->saveAccess(m_path);
    ctx->navigation->closeWindow();
  }

private:
  std::filesystem::path m_path;
};

inline std::unique_ptr<ActionPanelState> actionPanel(const std::filesystem::path &path, AppService *appDb) {
  QMimeDatabase mimeDb;
  auto panel = std::make_unique<ActionPanelState>();
  auto section = panel->createSection();
  auto mime = mimeDb.mimeTypeForFile(path.c_str());
  auto openers = appDb->findCuratedOpeners(mime.name());
  auto fileBrowser = appDb->fileBrowser();

  if (!openers.empty()) {
    auto open = new OpenFileAction(path, openers.front());
    section->addAction(open);
  }

  if (fileBrowser) { section->addAction(new RevealFileInFolderAction(path)); }

  auto suggested = panel->createSection("Suggested apps");

  for (size_t i = 1; i < openers.size(); ++i) {
    auto &opener = openers[i];
    if (fileBrowser && fileBrowser->id() == opener->id()) continue;
    auto open = new OpenFileAction(path, opener);
    suggested->addAction(open);
  }

  auto utils = panel->createSection();

  utils->addAction(new CopyToClipboardAction(Clipboard::Text(path.c_str()), "Copy file path"));
  utils->addAction(new CopyToClipboardAction(Clipboard::Text(path.filename().c_str()), "Copy file name"));

  if (mime.isValid()) {
    utils->addAction(new CopyToClipboardAction(Clipboard::Text(mime.name()), "Copy mime type"));
  }

  return panel;
}

} // namespace FileActions
