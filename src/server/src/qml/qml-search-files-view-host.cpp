#include "qml-search-files-view-host.hpp"
#include "qml-search-files-model.hpp"
#include "service-registry.hpp"
#include "services/files-service/file-service.hpp"
#include "utils/utils.hpp"
#include <QFileInfo>
#include <ranges>

namespace fs = std::filesystem;

QUrl QmlSearchFilesViewHost::qmlComponentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/SearchFilesView.qml"));
}

QVariantMap QmlSearchFilesViewHost::qmlProperties() const {
  return {{QStringLiteral("host"), QVariant::fromValue(this)}};
}

void QmlSearchFilesViewHost::initialize() {
  using namespace std::chrono_literals;
  BaseView::initialize();

  m_model = new QmlSearchFilesModel(this);
  m_model->initialize(context());

  setSearchPlaceholderText("Search for files...");

  m_debounce.setSingleShot(true);
  m_debounce.setInterval(100ms);
  connect(&m_debounce, &QTimer::timeout, this, &QmlSearchFilesViewHost::handleDebounce);
  connect(&m_pendingResults, &Watcher::finished, this, &QmlSearchFilesViewHost::handleSearchResults);
  connect(m_model, &QmlSearchFilesModel::fileSelected, this, &QmlSearchFilesViewHost::loadDetail);
}

void QmlSearchFilesViewHost::loadInitialData() { renderRecentFiles(); }

void QmlSearchFilesViewHost::textChanged(const QString &text) {
  if (m_pendingResults.isRunning()) m_pendingResults.cancel();

  if (text.isEmpty()) {
    m_debounce.stop();
    renderRecentFiles();
    return;
  }

  std::error_code ec;
  auto path = expandPath(text.trimmed().toStdString());
  if (path != "/" && fs::exists(path, ec)) {
    setLoading(false);
    m_model->setFiles({path}, QStringLiteral("Direct file path"));
    return;
  }

  setLoading(true);
  m_debounce.start();
}

void QmlSearchFilesViewHost::onReactivated() { m_model->refreshActionPanel(); }

QObject *QmlSearchFilesViewHost::listModel() const { return m_model; }

void QmlSearchFilesViewHost::renderRecentFiles() {
  auto fileService = context()->services->fileService();

  setLoading(false);
  auto recentFiles = fileService->getRecentlyAccessed() |
                     std::views::transform([](auto &&f) { return f.path; }) |
                     std::ranges::to<std::vector>();
  m_model->setFiles(std::move(recentFiles), QStringLiteral("Recently Accessed"));
}

void QmlSearchFilesViewHost::handleDebounce() {
  auto fileService = context()->services->fileService();
  QString query = searchText();

  if (m_pendingResults.isRunning()) m_pendingResults.cancel();

  if (query.isEmpty()) {
    setLoading(false);
    return;
  }

  m_lastSearchText = query;
  m_pendingResults.setFuture(fileService->queryAsync(query.toStdString()));
}

void QmlSearchFilesViewHost::handleSearchResults() {
  setLoading(false);

  if (!m_pendingResults.isFinished() || m_pendingResults.isCanceled()) return;
  if (searchText() != m_lastSearchText) return;

  auto results = m_pendingResults.result();
  auto paths = results | std::views::transform([](auto &&f) { return f.path; }) |
               std::ranges::to<std::vector>();
  m_model->setFiles(std::move(paths), QStringLiteral("Results"));
}

void QmlSearchFilesViewHost::loadDetail(const fs::path &path) {
  m_hasDetail = true;
  m_detailName = QString::fromStdString(getLastPathComponent(path));
  m_detailPath = QString::fromStdString(compressPath(path).string());

  auto mime = m_mimeDb.mimeTypeForFile(QString::fromStdString(path.string()));
  m_detailMimeType = mime.name();

  QFileInfo info(QString::fromStdString(path.string()));
  m_detailLastModified = info.lastModified().toString();

  m_detailImageSource.clear();
  m_detailTextContent.clear();

  if (mime.name().startsWith("image/")) {
    m_detailImageSource = qml::imageSourceFor(ImageURL::local(path));
  } else if (Utils::isTextMimeType(mime)) {
    QFile file(QString::fromStdString(path.string()));
    if (file.open(QIODevice::ReadOnly)) {
      static constexpr qint64 MAX_PREVIEW = 32 * 1024;
      m_detailTextContent = QString::fromUtf8(file.read(MAX_PREVIEW));
    }
  } else {
    m_detailImageSource =
        qml::imageSourceFor(ImageURL::system(mime.iconName()).withFallback(ImageURL::system(mime.genericIconName())));
  }

  emit detailChanged();
}

void QmlSearchFilesViewHost::clearDetail() {
  m_hasDetail = false;
  m_detailName.clear();
  m_detailPath.clear();
  m_detailMimeType.clear();
  m_detailLastModified.clear();
  m_detailImageSource.clear();
  m_detailTextContent.clear();
  emit detailChanged();
}
