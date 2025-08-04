#pragma once
#include "action-panel/action-panel.hpp"
#include "actions/app/app-actions.hpp"
#include "ui/views/base-view.hpp"
#include "clipboard-history-view.hpp"
#include "manage-quicklinks-command.hpp"
#include "services/files-service/file-service.hpp"
#include "../src/ui/image/url.hpp"
#include "service-registry.hpp"
#include "services/files-service/abstract-file-indexer.hpp"
#include "timer.hpp"
#include "ui/omni-list/omni-list.hpp"
#include "utils/utils.hpp"
#include <filesystem>
#include <qfuturewatcher.h>
#include <qlocale.h>
#include <qmimedatabase.h>

class FileListItemMetadata : public DetailWithMetadataWidget {
  std::filesystem::path m_path;
  QMimeDatabase m_mimeDb;

  std::vector<MetadataItem> createEntryMetadata(const std::filesystem::path &path) const {
    auto mimeType = m_mimeDb.mimeTypeForFile(path.c_str());
    auto stat = std::filesystem::status(path);
    QFileInfo info(path);

    auto lastModifiedAt = MetadataLabel{
        .text = info.lastModified().toString(),
        .title = "Last modified at",
    };
    auto mime = MetadataLabel{
        .text = mimeType.name(),
        .title = "Type",
    };
    auto name = MetadataLabel{
        .text = path.filename().c_str(),
        .title = "Name",
    };
    auto where = MetadataLabel{
        .text = compressPath(path).c_str(),
        .title = "Where",
    };

    return {name, where, mime, lastModifiedAt};
  }

  QWidget *createEntryWidget(const std::filesystem::path &path) {
    auto mime = m_mimeDb.mimeTypeForFile(path.c_str());

    if (mime.name().startsWith("image/")) {
      auto icon = new ImageWidget;

      icon->setContentsMargins(10, 10, 10, 10);
      icon->setUrl(ImageURL::local(path));

      return icon;
    }

    if (isTextMimeType(mime)) {
      auto container = new TextContainer;
      auto viewer = new TextFileViewer;

      container->setWidget(viewer);
      viewer->load(path.c_str());

      return container;
    }

    auto icon = new ImageWidget;

    icon->setContentsMargins(10, 10, 10, 10);
    icon->setUrl(ImageURL::system(mime.iconName()).withFallback(ImageURL::system(mime.genericIconName())));

    return icon;
  }

public:
  void setPath(const std::filesystem::path &path) {
    if (auto previous = content()) { previous->deleteLater(); }

    m_path = path;

    auto widget = createEntryWidget(path);
    auto metadata = createEntryMetadata(path);

    setContent(widget);
    setMetadata(metadata);
  }
};

class FileListItem : public AbstractDefaultListItem, public ListView::Actionnable {
  std::filesystem::path m_path;
  QMimeDatabase m_mimeDb;

  ImageURL getIcon() const {
    auto mime = m_mimeDb.mimeTypeForFile(m_path.c_str());

    if (!mime.name().isEmpty()) {
      if (!QIcon::fromTheme(mime.iconName()).isNull()) { return ImageURL::system(mime.iconName()); }

      return ImageURL::system(mime.genericIconName());
    }

    return ImageURL::builtin("question-mark-circle");
  }

  QWidget *generateDetail() const override {
    auto detail = new FileListItemMetadata();
    detail->setPath(m_path);

    return detail;
  }

  std::unique_ptr<ActionPanelState> newActionPanel(ApplicationContext *ctx) const override {
    auto panel = std::make_unique<ActionPanelState>();
    auto appDb = ctx->services->appDb();
    auto section = panel->createSection();
    auto openInFolder = new OpenAppAction(appDb->fileBrowser(), "Open in folder", {m_path.c_str()});

    if (auto app = appDb->findBestOpener(m_path.c_str())) {
      auto open = new OpenAppAction(app, "Open", {m_path.c_str()});
      open->setPrimary(true);
      section->addAction(open);
    } else {
      openInFolder->setPrimary(true);
    }

    section->addAction(openInFolder);

    return panel;
  }

public:
  QString generateId() const override { return m_path.c_str(); }

  ItemData data() const override {
    return {
        .iconUrl = getIcon(),
        .name = m_path.filename().c_str(),
    };
  }

  FileListItem(const std::filesystem::path &path) : m_path(path) {}
};

class SearchFilesView : public ListView {
  using Watcher = QFutureWatcher<std::vector<IndexerFileResult>>;
  Watcher m_pendingFileResults;
  QString m_lastSearchText;

  QString currentQuery;

  void initialize() override { setSearchPlaceholderText("Search for files..."); }

  void handleSearchResults() {
    if (!m_pendingFileResults.isFinished()) return;

    if (currentQuery != m_lastSearchText) return;

    auto results = m_pendingFileResults.result();

    m_list->updateModel([&]() {
      auto &section = m_list->addSection("Files");
      for (const auto &result : results) {
        section.addItem(std::make_unique<FileListItem>(result.path));
      }
    });
  }

  void generateFilteredList(const QString &query) {
    auto fileService = context()->services->fileService();

    currentQuery = query;
    if (m_pendingFileResults.isRunning()) { m_pendingFileResults.cancel(); }
    m_lastSearchText = query;
    m_pendingFileResults.setFuture(fileService->queryAsync(query.toStdString()));
  }

  void textChanged(const QString &query) override {
    if (!query.isEmpty()) generateFilteredList(query);
  }

public:
  SearchFilesView() {
    connect(&m_pendingFileResults, &Watcher::finished, this, &SearchFilesView::handleSearchResults);
  }
};
