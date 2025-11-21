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

  std::unique_ptr<ActionPanelState> newActionPanel(ApplicationContext *ctx) const override {
    auto panel = std::make_unique<ActionPanelState>();
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
      section->addAction(open);
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
