#include "search-files-view.hpp"
#include <filesystem>
#include <ranges>
#include "misc/file-list-item.hpp"
#include "ui/file-detail/file-detail.hpp"
#include "ui/views/base-view.hpp"
#include "services/files-service/file-service.hpp"
#include "service-registry.hpp"
#include "ui/views/typed-list-view.hpp"
#include "utils.hpp"

namespace fs = std::filesystem;

SearchFilesView::SearchFilesView() {
  using namespace std::chrono_literals;

  m_debounce.setSingleShot(true);
  m_debounce.setInterval(100ms);
  connect(&m_debounce, &QTimer::timeout, this, &SearchFilesView::handleDebounce);
  connect(&m_pendingFileResults, &Watcher::finished, this, &SearchFilesView::handleSearchResults);
}

std::unique_ptr<ActionPanelState> SearchFilesView::createActionPanel(const fs::path &item) const {
  return FileListItemBase::actionPanel(item, context()->services->appDb());
};

QWidget *SearchFilesView::generateDetail(const fs::path &path) const {
  auto detail = new FileDetail();
  detail->setPath(path);
  return detail;
}

void SearchFilesView::initialize() {
  TypedListView::initialize();
  m_model = new FileSearchModel();
  m_model->setParent(this);
  setModel(m_model);
  setSearchPlaceholderText("Search for files...");
  renderRecentFiles();
}

void SearchFilesView::textChanged(const QString &query) {
  currentQuery = query;

  if (m_pendingFileResults.isRunning()) { m_pendingFileResults.cancel(); }

  if (query.isEmpty()) {
    m_debounce.stop();
    renderRecentFiles();
  }
  generateFilteredList(query);
}

void SearchFilesView::renderRecentFiles() {
  auto fileService = context()->services->fileService();

  setLoading(false);
  m_model->setSectionName("Recently Accessed");
  m_model->setFiles(fileService->getRecentlyAccessed() |
                    std::views::transform([](auto &&f) { return f.path; }) | std::ranges::to<std::vector>());
  m_list->selectFirst();
}

void SearchFilesView::handleSearchResults() {
  setLoading(false);

  if (!m_pendingFileResults.isFinished() || m_pendingFileResults.isCanceled()) return;

  if (currentQuery != m_lastSearchText) return;

  auto results = m_pendingFileResults.result();

  m_model->setSectionName("Results");
  m_model->setFiles(results | std::views::transform([](auto &&f) { return f.path; }) |
                    std::ranges::to<std::vector>());
  m_list->selectFirst();
}

void SearchFilesView::handleDebounce() {
  auto fileService = context()->services->fileService();
  QString query = searchText();

  if (m_pendingFileResults.isRunning()) { m_pendingFileResults.cancel(); }

  if (query.isEmpty()) {
    setLoading(false);
    return;
  }

  m_lastSearchText = query;
  m_pendingFileResults.setFuture(fileService->queryAsync(query.toStdString()));
}

void SearchFilesView::generateFilteredList(const QString &query) {
  std::error_code ec;

  if (auto path = expandPath(query.toStdString()); path != "/" && fs::exists(path, ec)) {
    m_model->setSectionName("Direct file path");
    m_model->setFiles({path});
    m_list->selectFirst();
    return;
  }

  setLoading(true);
  m_debounce.start();
}
