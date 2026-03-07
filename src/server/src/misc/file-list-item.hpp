#pragma once
#include "actions/files/file-actions.hpp"
#include "clipboard-actions.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/app-service/app-service.hpp"
#include "services/toast/toast-service.hpp"
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QUrl>
#include <qmimedatabase.h>
#include <filesystem>
#include <memory>
#include <set>

namespace FileActions {

inline bool revealInFileManager(const std::filesystem::path &path) {
  auto const fileUrl = QUrl::fromLocalFile(path.c_str()).toString();
  QDBusInterface iface("org.freedesktop.FileManager1", "/org/freedesktop/FileManager1",
                       "org.freedesktop.FileManager1", QDBusConnection::sessionBus());

  if (!iface.isValid()) { return false; }

  QDBusReply<void> const reply = iface.call("ShowItems", QStringList{fileUrl}, QString{});
  return reply.isValid();
}

inline bool openInFolderFallback(const std::filesystem::path &path, AppService *appDb) {
  auto const browser = appDb->fileBrowser();
  if (!browser) { return false; }

  // These file managers don't work correctly when they're passed a file path
  // We work around this by passing the parent folder path instead
  static const std::set<QString> exceptions = {"org.kde.dolphin.desktop", "ranger.desktop"};

  if (!std::filesystem::exists(path)) {
    auto const parentPath = path.parent_path();
    return std::filesystem::exists(parentPath) && appDb->launch(*browser, {parentPath.c_str()});
  }

  if (exceptions.contains(browser->id())) { return appDb->launch(*browser, {path.parent_path().c_str()}); }

  return appDb->launch(*browser, {path.c_str()});
}

class RevealFileInFolderAction : public AbstractAction {
public:
  RevealFileInFolderAction(std::filesystem::path path)
      : AbstractAction("Open in folder", ImageURL::builtin("folder")), m_path(std::move(path)) {}

  void execute(ApplicationContext *ctx) override {
    auto const appDb = ctx->services->appDb();
    auto const files = ctx->services->fileService();
    auto const toast = ctx->services->toastService();

    bool const success = std::filesystem::exists(m_path)
                             ? revealInFileManager(m_path) || openInFolderFallback(m_path, appDb)
                             : openInFolderFallback(m_path, appDb);

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

  if (fileBrowser && (!openers.empty() && openers.front()->id() != fileBrowser->id())) {
    section->addAction(new RevealFileInFolderAction(path));
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
