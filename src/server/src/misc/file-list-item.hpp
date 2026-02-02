#pragma once
#include "actions/files/file-actions.hpp"
#include "clipboard-actions.hpp"
#include "common.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/app-service/app-service.hpp"
#include "utils.hpp"
#include <qmimedatabase.h>

/**
 * Base class for file results that are shown in the root search or in file search commands.
 */
class FileListItemBase : public AbstractDefaultListItem, public ListView::Actionnable {
protected:
  QMimeDatabase m_mimeDb;
  std::filesystem::path m_path;

  ImageURL getIcon() const { return ImageURL::fileIcon(m_path); }

  static OpenFileInAppAction *createOpenInFolderAction(const std::filesystem::path &path,
                                                       const std::shared_ptr<AbstractApplication> &browser) {

    // These file managers don't work correctly when they're passed a file path
    // We work around this by passing the parent folder path instead
    static const std::set<QString> exceptions = {"org.kde.dolphin.desktop", "ranger.desktop"};

    if (exceptions.contains(browser->id())) {
      return new OpenFileInAppAction(path, browser, "Open in folder", {path.parent_path().c_str()});
    }

    return new OpenFileInAppAction(path, browser, "Open in folder");
  }

public:
  static std::unique_ptr<ActionPanelState> actionPanel(const std::filesystem::path &path, AppService *appDb) {
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

    for (int i = 1; i < openers.size(); ++i) {
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

  std::unique_ptr<ActionPanelState> newActionPanel(ApplicationContext *ctx) const override {
    auto panel = std::make_unique<ListActionPanelState>();
    auto appDb = ctx->services->appDb();
    auto section = panel->createSection();
    auto mime = m_mimeDb.mimeTypeForFile(m_path.c_str());
    auto openers = appDb->findCuratedOpeners(mime.name());
    auto fileBrowser = appDb->fileBrowser();

    if (!openers.empty()) {
      auto open = new OpenFileAction(m_path, openers.front());
      section->addAction(open);
    }

    if (fileBrowser && (!openers.empty() && openers.front()->id() != fileBrowser->id())) {
      auto open = new OpenFileInAppAction(m_path, fileBrowser, "Open in folder");
      section->addAction(createOpenInFolderAction(m_path, fileBrowser));
    }

    auto suggested = panel->createSection("Suggested apps");

    for (int i = 1; i < openers.size(); ++i) {
      auto opener = openers[i];
      if (fileBrowser && fileBrowser->id() == opener->id()) continue;
      auto open = new OpenFileAction(m_path, opener);
      suggested->addAction(open);
    }

    auto utils = panel->createSection();

    utils->addAction(new CopyToClipboardAction(Clipboard::Text(m_path.c_str()), "Copy file path"));
    utils->addAction(new CopyToClipboardAction(Clipboard::Text(m_path.filename().c_str()), "Copy file name"));

    if (mime.isValid()) {
      utils->addAction(new CopyToClipboardAction(Clipboard::Text(mime.name()), "Copy mime type"));
    }

    return panel;
  }

public:
  QString generateId() const override { return m_path.c_str(); }

  ItemData data() const override {
    return {
        .iconUrl = getIcon(),
        .name = getLastPathComponent(m_path).c_str(),
    };
  }

  FileListItemBase(const std::filesystem::path &path) : m_path(path) {}
};
