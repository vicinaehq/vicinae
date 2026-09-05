#include "search-files-view-host.hpp"
#include "service-registry.hpp"
#include "services/files-service/file-service.hpp"
#include "utils/utils.hpp"
#include "view-utils.hpp"
#include <common/file-category.hpp>
#include <QFileInfo>
#include <ranges>

using namespace std::chrono_literals;

namespace fs = std::filesystem;

namespace {

bool isExplicitPathQuery(QStringView text) {
  if (text.isEmpty()) return false;

  if (text.startsWith(u'/') || text == u"~" || text.startsWith(u"~/")) return true;
  if (text == u"." || text == u"..") return true;
  if (text.startsWith(u"./") || text.startsWith(u"../")) return true;

#ifdef Q_OS_WIN
  if (text.startsWith(u'\\') || text.contains(u'\\')) return true;
  if (text.size() >= 3 && text.at(1) == u':' && (text.at(2) == u'/' || text.at(2) == u'\\')) { return true; }
#endif

  return false;
}

vicinae::FileCategory categoryForPath(const fs::path &path) {
  std::error_code ec;
  return vicinae::fileCategoryFor(path, fs::is_directory(path, ec));
}

} // namespace

QUrl SearchFilesViewHost::qmlComponentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/SearchFilesView.qml"));
}

QUrl SearchFilesViewHost::qmlSearchAccessoryUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/CategoryFilterAccessory.qml"));
}

QVariantMap SearchFilesViewHost::qmlProperties() {
  return {{QStringLiteral("host"), QVariant::fromValue(this)}};
}

void SearchFilesViewHost::initialize() {
  BaseView::initialize();
  initModel();

  m_section.setOnFileSelected([this](const fs::path &p) { loadDetail(p); });
  model()->addSource(&m_section);

  setSearchPlaceholderText(tr("Search for files..."));
  m_categoryFilterModel.setStringOptions(categoryFilterOptions());
  restoreCategoryFilter();

  m_debounce.setSingleShot(true);
  connect(&m_debounce, &QTimer::timeout, this, &SearchFilesViewHost::handleDebounce);
  connect(&m_pendingResults, &Watcher::finished, this, &SearchFilesViewHost::handleSearchResults);
}

void SearchFilesViewHost::loadInitialData() { renderEmptyQuery(); }

void SearchFilesViewHost::textChanged(const QString &text) {
  if (m_pendingResults.isRunning()) m_pendingResults.cancel();

  if (text.isEmpty()) {
    m_debounce.stop();
    renderEmptyQuery();
    return;
  }

  std::error_code ec;
  auto const trimmed = QStringView{text}.trimmed();
  auto path = expandPath(trimmed.toString().toStdString());
  if (isExplicitPathQuery(trimmed) && path != "/" && fs::exists(path, ec)) {
    m_debounce.stop();
    m_resultMode = ResultMode::DirectPath;
    setLoading(false);

    if (auto category = selectedCategory(); category && categoryForPath(path) != *category) {
      m_section.setFiles({}, tr("Direct file path"));
      return;
    }

    m_section.setFiles({path}, tr("Direct file path"));
    return;
  }

  auto const debounce = context()->services->fileService()->indexer()->queryDebounce();

  if (debounce == 0ms) {
    handleDebounce();
    return;
  }

  m_debounce.start(debounce);
}

void SearchFilesViewHost::renderRecentFiles() {
  auto fileService = context()->services->fileService();

  setLoading(false);
  auto recentFiles = fileService->getRecentlyAccessed() |
                     std::views::transform([](auto &&f) { return f.path; }) |
                     std::views::filter([&](const auto &path) {
                       auto category = selectedCategory();
                       return !category || categoryForPath(path) == *category;
                     }) |
                     std::ranges::to<std::vector>();
  m_section.setFiles(std::move(recentFiles), tr("Recently Accessed"));
}

void SearchFilesViewHost::renderEmptyQuery() {
  if (selectedCategory()) {
    startIndexedSearch({});
    return;
  }

  m_resultMode = ResultMode::Recent;
  renderRecentFiles();
}

void SearchFilesViewHost::startIndexedSearch(const QString &query) {
  auto fileService = context()->services->fileService();

  if (m_pendingResults.isRunning()) m_pendingResults.cancel();

  m_lastSearchText = query;
  m_resultMode = ResultMode::IndexedSearch;
  setLoading(true);
  m_pendingResults.setFuture(fileService->queryAsync(query.toStdString(), {.category = selectedCategory()}));
}

void SearchFilesViewHost::handleDebounce() {
  QString const query = searchText();

  if (query.isEmpty()) {
    renderEmptyQuery();
    return;
  }

  startIndexedSearch(query);
}

void SearchFilesViewHost::handleSearchResults() {
  if (!m_pendingResults.isFinished() || m_pendingResults.isCanceled()) return;
  if (m_resultMode != ResultMode::IndexedSearch) return;
  if (searchText() != m_lastSearchText) return;

  setLoading(false);
  auto results = m_pendingResults.result();
  auto paths =
      results | std::views::transform([](auto &&f) { return f.path; }) | std::ranges::to<std::vector>();
  m_section.setFiles(std::move(paths), m_lastSearchText.isEmpty() ? tr("Recently Modified") : tr("Results"));
}

void SearchFilesViewHost::loadDetail(const fs::path &path) {
  m_hasDetail = true;
  m_detailName = QString::fromStdString(getLastPathComponent(path));
  m_detailPath = QString::fromStdString(compressPath(path).string());

  QFileInfo const info(QString::fromStdString(path.string()));
  m_detailLastModified = info.lastModified().toString();

  auto preview = qml::resolveFilePreview(path, m_mimeDb);
  m_detailMimeType = preview.mimeType;
  m_detailImageSource = preview.imageSource;
  m_detailTextContent = preview.textContent;

  emit detailChanged();
}

void SearchFilesViewHost::clearDetail() {
  m_hasDetail = false;
  m_detailName.clear();
  m_detailPath.clear();
  m_detailMimeType.clear();
  m_detailLastModified.clear();
  m_detailImageSource.clear();
  m_detailTextContent.clear();
  emit detailChanged();
}

static const QStringList &categoryFilterKeys() {
  static const QStringList keys = {
      QStringLiteral("All"),       QStringLiteral("Other"),    QStringLiteral("Directories"),
      QStringLiteral("Images"),    QStringLiteral("Videos"),   QStringLiteral("Audio"),
      QStringLiteral("Documents"), QStringLiteral("Archives"), QStringLiteral("Applications"),
  };
  return keys;
}

QStringList SearchFilesViewHost::categoryFilterOptions() const {
  return {tr("All"),   tr("Other"),     tr("Directories"), tr("Images"),      tr("Videos"),
          tr("Audio"), tr("Documents"), tr("Archives"),    tr("Applications")};
}

void SearchFilesViewHost::setCategoryFilter(int index) {
  if (index < 0 || index >= categoryFilterOptions().size()) return;
  if (index == m_currentCategoryFilter) return;

  m_currentCategoryFilter = index;
  emit currentCategoryFilterChanged();

  command()->storage().setItem("fileCategory", categoryFilterKeys().value(index));

  if (searchText().isEmpty()) {
    renderEmptyQuery();
  } else {
    textChanged(searchText());
  }
}

void SearchFilesViewHost::restoreCategoryFilter() {
  const auto saved = command()->storage().getItem("fileCategory");
  if (saved.isUndefined() || saved.isNull()) return;

  const int index = categoryFilterKeys().indexOf(saved.toString());
  if (index <= 0) return;

  m_currentCategoryFilter = index;
  emit currentCategoryFilterChanged();
}

std::optional<vicinae::FileCategory> SearchFilesViewHost::selectedCategory() const {
  switch (m_currentCategoryFilter) {
  case 1:
    return vicinae::FileCategory::Other;
  case 2:
    return vicinae::FileCategory::Directory;
  case 3:
    return vicinae::FileCategory::Image;
  case 4:
    return vicinae::FileCategory::Video;
  case 5:
    return vicinae::FileCategory::Audio;
  case 6:
    return vicinae::FileCategory::Document;
  case 7:
    return vicinae::FileCategory::Archive;
  case 8:
    return vicinae::FileCategory::Application;
  default:
    return std::nullopt;
  }
}
