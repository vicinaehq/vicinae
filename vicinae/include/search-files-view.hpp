#pragma once
#include "misc/file-list-item.hpp"
#include "ui/file-detail/file-detail.hpp"
#include "ui/views/base-view.hpp"
#include "services/files-service/file-service.hpp"
#include "service-registry.hpp"
#include "services/files-service/abstract-file-indexer.hpp"
#include "ui/omni-list/omni-list.hpp"
#include <filesystem>
#include <qfuturewatcher.h>
#include <qlocale.h>
#include <qmimedatabase.h>

class FileListItem : public FileListItemBase {
  QWidget *generateDetail() const override {
    auto detail = new FileDetail();
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

  void generateFilteredList(const QString &query) {
    auto fileService = context()->services->fileService();

    if (m_pendingFileResults.isRunning()) { m_pendingFileResults.cancel(); }
    m_lastSearchText = query;
    setLoading(true);
    m_pendingFileResults.setFuture(fileService->queryAsync(query.toStdString()));
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
    connect(&m_pendingFileResults, &Watcher::finished, this, &SearchFilesView::handleSearchResults);
  }
};
