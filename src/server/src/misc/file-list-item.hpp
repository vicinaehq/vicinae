#pragma once
#include "actions/files/file-actions.hpp"
#include "clipboard-actions.hpp"
#include "navigation-controller.hpp"
#include "services/app-service/app-service.hpp"
#include <qmimedatabase.h>
#include <filesystem>
#include <memory>
#include <set>

namespace FileActions {

inline OpenFileInAppAction *createOpenInFolderAction(const std::filesystem::path &path,
                                                     const std::shared_ptr<AbstractApplication> &browser) {
  // These file managers don't work correctly when they're passed a file path
  // We work around this by passing the parent folder path instead
  static const std::set<QString> exceptions = {"org.kde.dolphin.desktop", "ranger.desktop"};

  if (exceptions.contains(browser->id())) {
    return new OpenFileInAppAction(path, browser, "Open in folder", {path.parent_path().c_str()});
  }

  return new OpenFileInAppAction(path, browser, "Open in folder");
}

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

  if (fileBrowser && (!openers.empty() && openers.front()->id() != fileBrowser->id())) {
    section->addAction(createOpenInFolderAction(path, fileBrowser));
  }

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
