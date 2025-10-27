#pragma once
#include "layout.hpp"
#include "misc/file-list-item.hpp"
#include "ui/views/base-view.hpp"
#include "manage-quicklinks-command.hpp"
#include "services/files-service/file-service.hpp"
#include "ui/image/url.hpp"
#include "service-registry.hpp"
#include "services/files-service/abstract-file-indexer.hpp"
#include "services/files-service/file-indexer/file-indexer.hpp"
#include "ui/omni-list/omni-list.hpp"
#include "utils/utils.hpp"
#include <filesystem>
#include <qfuturewatcher.h>
#include <qlocale.h>
#include <qmimedatabase.h>
#include <qtimer.h>
#include "ui/text-file-viewer/text-file-viewer.hpp"

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

    if (Utils::isTextMimeType(mime)) {
      auto viewer = new TextFileViewer;
      viewer->load(path);

      return VStack().add(viewer).buildWidget();
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

class FileListItem : public FileListItemBase {
  QWidget *generateDetail() const override {
    auto detail = new FileListItemMetadata();
    detail->setPath(m_path);

    return detail;
  }

public:
  QString generateId() const override { return m_path.c_str(); }

  ItemData data() const override {
    return {
        .iconUrl = getIcon(),
        .name = m_path.filename().c_str(),
    };
  }

  FileListItem(const std::filesystem::path &path) : FileListItemBase(path) {}
};

class SearchFilesView : public ListView {
  using Watcher = QFutureWatcher<std::vector<IndexerFileResult>>;
  Watcher m_pendingFileResults;
  QString m_lastSearchText;
  QString currentQuery;
  QTimer m_debounceTimer;

  void initialize() override {
    setSearchPlaceholderText("Search for files...");
    renderRecentFiles();
  }

  void handleSearchResults() {
    setLoading(false);
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

  void executeSearch() {
    auto fileService = context()->services->fileService();

    if (m_pendingFileResults.isRunning()) { m_pendingFileResults.cancel(); }
    m_lastSearchText = currentQuery;
    setLoading(true);
    m_pendingFileResults.setFuture(fileService->queryAsync(currentQuery.toStdString()));
  }

  void generateFilteredList(const QString &query) {
    auto fileService = context()->services->fileService();
    auto indexer = dynamic_cast<FileIndexer *>(fileService->indexer());

    if (indexer && indexer->useRegex()) {
      m_debounceTimer.start();
    } else {
      executeSearch();
    }
  }

  void renderRecentFiles() {
    auto fileService = context()->services->fileService();

    m_list->updateModel([&]() {
      auto &section = m_list->addSection("Recently Accessed");
      for (const auto &file : fileService->getRecentlyAccessed()) {
        section.addItem(std::make_unique<FileListItem>(file.path));
      }
    });
  }

  void textChanged(const QString &query) override {
    currentQuery = query;
    if (query.isEmpty()) return renderRecentFiles();
    generateFilteredList(query);
  }

public:
  SearchFilesView() {
    // debounce is important for regex searches to avoid excessive computation, could be removed or reduced
    // for simple searches
    m_debounceTimer.setSingleShot(true);
    m_debounceTimer.setInterval(std::chrono::milliseconds(500));
    connect(&m_debounceTimer, &QTimer::timeout, this, &SearchFilesView::executeSearch);
    connect(&m_pendingFileResults, &Watcher::finished, this, &SearchFilesView::handleSearchResults);
  }
};
