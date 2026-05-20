#include "search-files-view-host.hpp"
#include "service-registry.hpp"
#include "services/files-service/file-service.hpp"
#include "utils/utils.hpp"
#include "view-utils.hpp"
#include <QFileInfo>
#include <QLocale>
#include <ranges>

namespace fs = std::filesystem;

QUrl SearchFilesViewHost::qmlComponentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/SearchFilesView.qml"));
}

QVariantMap SearchFilesViewHost::qmlProperties() {
  return {{QStringLiteral("host"), QVariant::fromValue(this)}};
}

void SearchFilesViewHost::initialize() {
  using namespace std::chrono_literals;
  BaseView::initialize();
  initModel();

  m_section.setOnFileSelected([this](const fs::path &p) { loadDetail(p); });
  model()->addSource(&m_section);
  setSearchPlaceholderText("Search for files...");

  m_debounce.setSingleShot(true);
  m_debounce.setInterval(32ms); // fff is fast enough to run even in 60 fps
  connect(&m_debounce, &QTimer::timeout, this, &SearchFilesViewHost::handleDebounce);
  connect(&m_pendingResults, &Watcher::finished, this, &SearchFilesViewHost::handleSearchResults);

  m_readyPulseTimer.setSingleShot(true);
  m_readyPulseTimer.setInterval(2000);
  connect(&m_readyPulseTimer, &QTimer::timeout, this, [this]() {
    if (!m_showReadyPulse) return;
    m_showReadyPulse = false;
    emit indexingStateChanged();
    if (searchText().isEmpty()) renderRecentFiles();
  });

  if (auto *fileService = context()->services->fileService()) {
    if (auto *indexer = fileService->indexer()) {
      connect(indexer, &AbstractFileIndexer::scanStateChanged, this, &SearchFilesViewHost::handleScanState);
      // Seed local state in case the view is re-entered after the index is
      // already ready.
      auto state = indexer->scanState();
      handleScanState(static_cast<quint64>(state.scannedFilesCount), state.isScanning, state.isReady);
    }
  }
}

void SearchFilesViewHost::loadInitialData() {
  // Entering the view is the trigger for indexing. start() is idempotent.
  if (auto *fileService = context()->services->fileService()) {
    if (auto *indexer = fileService->indexer()) indexer->start();
  }

  if (m_isIndexing || m_showReadyPulse) {
    clearSection();
    return;
  }
  renderRecentFiles();
}

void SearchFilesViewHost::textChanged(const QString &text) {
  if (m_pendingResults.isRunning()) m_pendingResults.cancel();

  if (text.isEmpty()) {
    m_debounce.stop();
    if (m_isIndexing || m_showReadyPulse) {
      clearSection();
    } else {
      renderRecentFiles();
    }
    return;
  }

  std::error_code ec;
  auto path = expandPath(text.trimmed().toStdString());
  if (path != "/" && fs::exists(path, ec)) {
    setLoading(false);
    m_section.setFiles({path}, QStringLiteral("Direct file path"));
    return;
  }

  setLoading(true);
  m_debounce.start();
}

void SearchFilesViewHost::renderRecentFiles() {
  auto fileService = context()->services->fileService();

  setLoading(false);
  auto recentFiles = fileService->getRecentlyAccessed() |
                     std::views::transform([](auto &&f) { return f.path; }) | std::ranges::to<std::vector>();
  m_section.setFiles(std::move(recentFiles), QStringLiteral("Recently Accessed"));
}

void SearchFilesViewHost::clearSection() {
  setLoading(false);
  m_section.setFiles({}, QString());
}

void SearchFilesViewHost::handleDebounce() {
  auto fileService = context()->services->fileService();
  QString const query = searchText();

  if (m_pendingResults.isRunning()) m_pendingResults.cancel();

  if (query.isEmpty()) {
    setLoading(false);
    return;
  }

  m_lastSearchText = query;
  m_pendingResults.setFuture(fileService->queryAsync(query.toStdString()));
}

void SearchFilesViewHost::handleSearchResults() {
  setLoading(false);

  if (!m_pendingResults.isFinished() || m_pendingResults.isCanceled()) return;
  if (searchText() != m_lastSearchText) return;

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

void SearchFilesViewHost::handleScanState(quint64 scanned, bool scanning, bool ready) {
  bool changed = false;
  if (m_indexedFilesCount != scanned) {
    m_indexedFilesCount = scanned;
    changed = true;
  }

  bool const indexing = scanning && !ready;
  if (m_isIndexing != indexing) {
    m_isIndexing = indexing;
    changed = true;
  }

  bool const wasReady = m_indexReady;
  if (m_indexReady != ready) {
    m_indexReady = ready;
    changed = true;
  }

  if (!wasReady && ready) {
    m_readyAnnounceCount = scanned;
    m_showReadyPulse = true;
    clearSection();
    m_readyPulseTimer.start();
    changed = true;
  }

  if (wasReady && !ready) {
    if (m_showReadyPulse) {
      m_showReadyPulse = false;
      m_readyPulseTimer.stop();
      changed = true;
    }
    if (searchText().isEmpty()) renderRecentFiles();
  }

  if (changed) emit indexingStateChanged();
}

QString SearchFilesViewHost::emptyTitle() const {
  if (m_isIndexing) return QStringLiteral("Indexing your $HOME");
  if (m_showReadyPulse) return QStringLiteral("Ready to search");
  return QStringLiteral("No results");
}

QString SearchFilesViewHost::emptyDescription() const {
  if (m_isIndexing) {
    if (m_indexedFilesCount == 0) return QStringLiteral("Starting up — this only happens once.");
    auto count = QLocale::system().toString(m_indexedFilesCount);
    return QStringLiteral("%1 files scanned so far").arg(count);
  }
  if (m_showReadyPulse) {
    auto count = QLocale::system().toString(m_readyAnnounceCount);
    return QStringLiteral("Indexed %1 files. Type to search.").arg(count);
  }
  return QString();
}
