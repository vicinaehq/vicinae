#include "search-files-view.hpp"
#include "ui/omni-list/omni-list.hpp"
#include <filesystem>
#include "misc/file-list-item.hpp"
#include "ui/file-detail/file-detail.hpp"
#include "ui/views/base-view.hpp"
#include "services/files-service/file-service.hpp"
#include "service-registry.hpp"
#include "utils.hpp"

namespace fs = std::filesystem;

class FileListItem : public FileListItemBase {
  QWidget *generateDetail() const override {
    auto detail = new FileDetail();
    detail->setPath(m_path);

    return detail;
  }

public:
  QString generateId() const override { return m_path.c_str(); }

  ItemData data() const override {
    return {.iconUrl = getIcon(), .name = getLastPathComponent(m_path).c_str()};
  }

  FileListItem(const std::filesystem::path &path) : FileListItemBase(path) {}
};

SearchFilesView::SearchFilesView() {
  connect(&m_pendingFileResults, &Watcher::finished, this, &SearchFilesView::handleSearchResults);
}

void SearchFilesView::initialize() {
  setSearchPlaceholderText("Search for files...");
  renderRecentFiles();
}

void SearchFilesView::textChanged(const QString &query) {
  currentQuery = query;
  if (query.isEmpty()) return renderRecentFiles();
  generateFilteredList(query);
}

void SearchFilesView::renderRecentFiles() {
  auto fileService = context()->services->fileService();

  m_list->updateModel([&]() {
    auto &section = m_list->addSection("Recently Accessed");
    for (const auto &file : fileService->getRecentlyAccessed()) {
      section.addItem(std::make_unique<FileListItem>(file.path));
    }
  });
}

void SearchFilesView::handleSearchResults() {
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

void SearchFilesView::generateFilteredList(const QString &query) {
  std::error_code ec;

  if (auto path = expandPath(query.toStdString()); path != "/" && fs::exists(path, ec)) {
    m_list->updateModel([&]() {
      auto &section = m_list->addSection("Files");
      section.addItem(std::make_unique<FileListItem>(path));
    });
    return;
  }

  auto fileService = context()->services->fileService();

  if (m_pendingFileResults.isRunning()) { m_pendingFileResults.cancel(); }
  m_lastSearchText = query;
  setLoading(true);
  m_pendingFileResults.setFuture(fileService->queryAsync(query.toStdString()));
}
