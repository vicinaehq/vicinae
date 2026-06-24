#pragma once
#include "actions/files/file-actions.hpp"
#include "clipboard-actions.hpp"
#include "common/context.hpp"
#include "keyboard/keybind.hpp"
#include "keyboard/keyboard.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/app-service/app-service.hpp"
#include "internal/keyboard/keyboard.hpp"
#include "services/toast/toast-service.hpp"
#include <qmimedatabase.h>
#include <filesystem>
#include <memory>

namespace FileActions {

class RevealFileInFolderAction : public AbstractAction {
public:
  RevealFileInFolderAction(std::filesystem::path path)
      : AbstractAction("Show in file browser", ImageURL::builtin("folder")), m_path(std::move(path)) {
    setShortcut(Keyboard::Shortcut::submit());
  }

  void execute(ApplicationContext *ctx) override {
    auto const appDb = ctx->services->appDb();
    auto const files = ctx->services->fileService();
    auto const toast = ctx->services->toastService();

    bool const success = appDb->showInFileBrowser(m_path, true);

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

inline std::unique_ptr<ActionPanelState> actionPanel(const std::filesystem::path &path,
                                                     const ApplicationContext *ctx) {
  QMimeDatabase mimeDb;
  auto panel = std::make_unique<ListActionPanelState>();
  auto section = panel->createSection();
  auto mime = mimeDb.mimeTypeForFile(path.c_str());
  auto appDb = ctx->services->appDb();
  auto pasteService = ctx->services->pasteService();
  auto openers = appDb->findCuratedOpeners(QString::fromStdString(path.string()));
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
  auto copy = AbstractAction::make<CopyToClipboardAction>(Clipboard::File(path), "Copy file");
  auto copyPath =
      AbstractAction::make<CopyToClipboardAction>(Clipboard::Text(path.c_str()), "Copy file path");
  auto copyFileName =
      AbstractAction::make<CopyToClipboardAction>(Clipboard::Text(path.filename().c_str()), "Copy file name");

  copy->setShortcut(Keybind::CopyAction);

  if (pasteService->supportsPaste()) {
    auto paste = AbstractAction::make<PasteToFocusedWindowAction>(Clipboard::File(path));
    paste->setShortcut(Keyboard::Shortcut::osPaste().shifted());
    utils->addAction(std::move(paste));
  }

  // TODO: for some reason those are broken, we need to investigate why
  // copyPath->setShortcut(Keybind::CopyPathAction);
  // copyFileName->setShortcut(Keybind::CopyNameAction);

  utils->addAction(std::move(copy));
  utils->addAction(std::move(copyPath));
  utils->addAction(std::move(copyFileName));

  if (mime.isValid()) {
    utils->addAction(new CopyToClipboardAction(Clipboard::Text(mime.name()), "Copy mime type"));
  }

  return panel;
}

} // namespace FileActions
