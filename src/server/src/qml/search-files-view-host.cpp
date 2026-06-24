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

constexpr auto FILE_SEARCH_DEBOUNCE = 100ms;

namespace {

QString storageKeyForCommand(QStringView commandId) {
  return QStringLiteral("fileCategory:%1").arg(commandId);
}

QString legacyStorageKey() { return QStringLiteral("fileCategory"); }

std::optional<vicinae::FileCategory> defaultCategoryForCommandId(QStringView commandId) {
  if (commandId == u"search-directories") return vicinae::FileCategory::Directory;
  if (commandId == u"search-images") return vicinae::FileCategory::Image;
  if (commandId == u"search-videos") return vicinae::FileCategory::Video;
  if (commandId == u"search-audio") return vicinae::FileCategory::Audio;
  if (commandId == u"search-documents") return vicinae::FileCategory::Document;
  if (commandId == u"search-archives") return vicinae::FileCategory::Archive;
  if (commandId == u"search-applications") return vicinae::FileCategory::Application;
  return std::nullopt;
}

int categoryIndexFor(vicinae::FileCategory category) {
  switch (category) {
  case vicinae::FileCategory::Other:
    return 1;
  case vicinae::FileCategory::Directory:
    return 2;
  case vicinae::FileCategory::Image:
    return 3;
  case vicinae::FileCategory::Video:
    return 4;
  case vicinae::FileCategory::Audio:
    return 5;
  case vicinae::FileCategory::Document:
    return 6;
  case vicinae::FileCategory::Archive:
    return 7;
  case vicinae::FileCategory::Application:
    return 8;
  default:
    return 0;
  }
}

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

  setSearchPlaceholderText("Search for files...");
  restoreCategoryFilter();

  if (m_currentCategoryFilter == 0) {
    if (auto category = defaultCategoryForCommandId(command()->info().commandId())) {
      m_currentCategoryFilter = categoryIndexFor(*category);
      emit currentCategoryFilterChanged();
    }
  }

  m_debounce.setSingleShot(true);
  m_debounce.setInterval(FILE_SEARCH_DEBOUNCE);
  connect(&m_debounce, &QTimer::timeout, this, &SearchFilesViewHost::handleDebounce);
  connect(&m_pendingResults, &Watcher::finished, this, &SearchFilesViewHost::handleSearchResults);
}

void SearchFilesViewHost::loadInitialData() { renderRecentFiles(); }

void SearchFilesViewHost::textChanged(const QString &text) {
  if (m_pendingResults.isRunning()) m_pendingResults.cancel();

  if (text.isEmpty()) {
    m_debounce.stop();
    m_resultMode = ResultMode::Recent;
    renderRecentFiles();
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
      m_section.setFiles({}, QStringLiteral("Direct file path"));
      return;
    }

    m_section.setFiles({path}, QStringLiteral("Direct file path"));
    return;
  }

  m_debounce.start();
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
  m_section.setFiles(std::move(recentFiles), QStringLiteral("Recently Accessed"));
}

void SearchFilesViewHost::handleDebounce() {
  auto fileService = context()->services->fileService();
  QString const query = searchText();

  if (m_pendingResults.isRunning()) m_pendingResults.cancel();

  if (query.isEmpty()) {
    m_resultMode = ResultMode::Recent;
    setLoading(false);
    return;
  }

  m_lastSearchText = query;
  m_resultMode = ResultMode::IndexedSearch;
  setLoading(true);
  m_pendingResults.setFuture(fileService->queryAsync(query.toStdString(), {.category = selectedCategory()}));
}

void SearchFilesViewHost::handleSearchResults() {
  if (!m_pendingResults.isFinished() || m_pendingResults.isCanceled()) return;
  if (m_resultMode != ResultMode::IndexedSearch) return;
  if (searchText() != m_lastSearchText) return;

  setLoading(false);
  auto results = m_pendingResults.result();
  auto paths =
      results | std::views::transform([](auto &&f) { return f.path; }) | std::ranges::to<std::vector>();
  m_section.setFiles(std::move(paths), QStringLiteral("Results"));
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

QStringList SearchFilesViewHost::categoryFilterOptions() const {
  return {
      QStringLiteral("All"),       QStringLiteral("Other"),    QStringLiteral("Directories"),
      QStringLiteral("Images"),    QStringLiteral("Videos"),   QStringLiteral("Audio"),
      QStringLiteral("Documents"), QStringLiteral("Archives"), QStringLiteral("Applications"),
  };
}

void SearchFilesViewHost::setCategoryFilter(int index) {
  if (index < 0 || index >= categoryFilterOptions().size()) return;
  if (index == m_currentCategoryFilter) return;

  m_currentCategoryFilter = index;
  emit currentCategoryFilterChanged();

  auto storage = command()->storage();
  auto const value = categoryFilterOptions().value(index);
  storage.setItem(storageKeyForCommand(command()->info().commandId()), value);
  if (command()->info().commandId() == QStringLiteral("search")) {
    storage.setItem(legacyStorageKey(), value);
  }

  if (searchText().isEmpty()) {
    renderRecentFiles();
  } else {
    textChanged(searchText());
  }
}

void SearchFilesViewHost::restoreCategoryFilter() {
  auto storage = command()->storage();
  auto saved = storage.getItem(storageKeyForCommand(command()->info().commandId()));

  if ((saved.isUndefined() || saved.isNull()) && command()->info().commandId() == QStringLiteral("search")) {
    saved = storage.getItem(legacyStorageKey());
  }

  if (saved.isUndefined() || saved.isNull()) return;

  const int index = categoryFilterOptions().indexOf(saved.toString());
  if (index > 0) setCategoryFilter(index);
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
