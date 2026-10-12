#include "builtins/clipboard/history/clipboard-history-view-host.hpp"
#include "builtins/clipboard/clipboard-preferences.hpp"
#include "builtins/clipboard/history/clipboard-history-controller.hpp"
#include "ui/views/view-utils.hpp"
#include "service-registry.hpp"
#include "services/clipboard/clipboard-service.hpp"
#include "services/paste/paste-service.hpp"
#include "utils/utils.hpp"
#include "vicinae.hpp"
#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QUrl>

static QString kindLabel(ClipboardOfferKind kind) {
  switch (kind) {
  case ClipboardOfferKind::Text:
    return QCoreApplication::translate("clipboard-history-view-host", "Text");
  case ClipboardOfferKind::Link:
    return QCoreApplication::translate("clipboard-history-view-host", "Link");
  case ClipboardOfferKind::Image:
    return QCoreApplication::translate("clipboard-history-view-host", "Image");
  case ClipboardOfferKind::File:
    return QCoreApplication::translate("clipboard-history-view-host", "File");
  case ClipboardOfferKind::Unknown:
  case ClipboardOfferKind::Count:
    break;
  }
  return QCoreApplication::translate("clipboard-history-view-host", "Unknown");
}

static std::optional<ClipboardOfferKind> kindFromFilterIndex(int index) {
  switch (index) {
  case 1:
    return ClipboardOfferKind::Text;
  case 2:
    return ClipboardOfferKind::Image;
  case 3:
    return ClipboardOfferKind::Link;
  case 4:
    return ClipboardOfferKind::File;
  default:
    return std::nullopt;
  }
}

static int filterIndexFromKind(std::optional<ClipboardOfferKind> kind) {
  if (!kind) return 0;
  switch (*kind) {
  case ClipboardOfferKind::Text:
    return 1;
  case ClipboardOfferKind::Image:
    return 2;
  case ClipboardOfferKind::Link:
    return 3;
  case ClipboardOfferKind::File:
    return 4;
  default:
    return 0;
  }
}

static const std::unordered_map<QString, ClipboardOfferKind> savedFilterToKind{
    {"text", ClipboardOfferKind::Text},
    {"image", ClipboardOfferKind::Image},
    {"link", ClipboardOfferKind::Link},
    {"file", ClipboardOfferKind::File},
};

static const char *filterIndexToSavedValue[] = {"all", "text", "image", "link", "file"};

ClipboardHistoryViewHost::ClipboardHistoryViewHost() : ViewHostBase() {
  m_kindFilterModel.setStringOptions({tr("All"), tr("Text"), tr("Images"), tr("Links"), tr("Files")});
}

ClipboardHistoryViewHost::~ClipboardHistoryViewHost() {
  if (m_clipman) m_clipman->resumeEviction();
}

QUrl ClipboardHistoryViewHost::qmlComponentUrl() const { return qml::componentUrl(u"ClipboardHistoryView"); }

QUrl ClipboardHistoryViewHost::qmlSearchAccessoryUrl() const {
  return qml::componentUrl(u"ClipboardFilterAccessory");
}

QVariantMap ClipboardHistoryViewHost::qmlProperties() {
  return {{QStringLiteral("host"), QVariant::fromValue(this)}};
}

void ClipboardHistoryViewHost::initialize() {
  BaseView::initialize();

  m_clipman = context()->services->clipman();
  m_clipman->pauseEviction();
  m_model.setScope(ViewScope(context(), this));

  m_section.setOnEntrySelected([this](const ClipboardHistoryEntry &e) { loadDetail(e); });
  m_model.addSource(&m_section);

  m_controller = new ClipboardHistoryController(m_clipman, this);

  const auto defaultAction = command()->preferences<ClipboardHistoryPreferences>().defaultAction;
  m_section.setDefaultAction(context()->services->pasteService()->supportsPaste()
                                 ? defaultAction
                                 : ClipboardHistorySection::DefaultAction::Copy);

  setSearchPlaceholderText(tr("Browse clipboard history..."));

  m_canToggleMonitoring = m_clipman->supportsMonitoring();
  if (!m_canToggleMonitoring) {
    m_clipboardStatusText = tr("Clipboard monitoring unavailable");
    m_clipboardStatusIcon =
        qml::imageSourceFor(ImageURL::builtin(BuiltinIcon::Warning).setFill(SemanticColor::Red));
  } else {
    handleMonitoringChanged(m_clipman->monitoring());
  }

  connect(m_clipman, &ClipboardService::monitoringChanged, this,
          &ClipboardHistoryViewHost::handleMonitoringChanged);

  connect(m_controller, &ClipboardHistoryController::dataRetrieved, this,
          [this](const PaginatedResponse<ClipboardHistoryEntry> &page) {
            bool const incremental = !m_model.selectFirstOnReset();
            m_section.setEntries(page);
            m_model.setSelectFirstOnReset(false);
            if (incremental) m_model.refreshActionPanel();
            if (std::exchange(m_detailStale, false)) reloadDetail();
            handleDataRetrieved(page.totalCount);
          });

  connect(m_controller, &ClipboardHistoryController::dataLoadingChanged, this, &BaseView::setLoading);

  // the detail is reloaded once the list has the updated entry
  connect(m_clipman, &ClipboardService::selectionContentChanged, this, [this](const QString &id) {
    if (m_hasDetail && id == m_detailEntryId) m_detailStale = true;
  });

  m_detailFileDebounce.setSingleShot(true);
  m_detailFileDebounce.setInterval(DETAIL_FILE_DEBOUNCE);
  connect(&m_detailFileDebounce, &QTimer::timeout, this, &ClipboardHistoryViewHost::reloadChangedDetailFile);
  connect(&m_detailFileWatcher, &QFileSystemWatcher::fileChanged, &m_detailFileDebounce,
          qOverload<>(&QTimer::start));
  connect(&m_detailFileWatcher, &QFileSystemWatcher::directoryChanged, &m_detailFileDebounce,
          qOverload<>(&QTimer::start));

  auto savedFilter = getSavedDropdownFilter().value_or("all");
  if (auto it = savedFilterToKind.find(savedFilter); it != savedFilterToKind.end()) {
    m_currentKindFilter = filterIndexFromKind(it->second);
    m_controller->setKindFilter(it->second);
  }
}

void ClipboardHistoryViewHost::loadInitialData() {
  m_controller->setFilter(searchText());
  updateSearchTerms(searchText());
}

void ClipboardHistoryViewHost::textChanged(const QString &text) {
  m_model.setSelectFirstOnReset(true);
  m_controller->setFilter(text);
  updateSearchTerms(text);
}

void ClipboardHistoryViewHost::updateSearchTerms(const QString &text) {
  auto terms = ClipboardDatabase::searchTerms(text);
  if (terms == m_searchTerms) return;
  m_searchTerms = std::move(terms);
  emit searchTermsChanged();
}

void ClipboardHistoryViewHost::onReactivated() { m_model.refreshActionPanel(); }

void ClipboardHistoryViewHost::beforePop() { m_model.beforePop(); }

void ClipboardHistoryViewHost::toggleMonitoring() {
  command()->setPreferenceValues({{"monitoring", !m_clipman->monitoring()}});
}

void ClipboardHistoryViewHost::setKindFilter(int kind) {
  if (m_currentKindFilter == kind) return;
  m_currentKindFilter = kind;
  emit currentKindFilterChanged();

  auto offerKind = kindFromFilterIndex(kind);
  m_model.setSelectFirstOnReset(true);
  m_controller->setKindFilter(offerKind);

  if (kind >= 0 && kind <= 4) { saveDropdownFilter(filterIndexToSavedValue[kind]); }

  if (!searchText().isEmpty()) { clearSearchText(); }
}

void ClipboardHistoryViewHost::handleMonitoringChanged(bool monitoring) {
  if (monitoring) {
    m_clipboardStatusText = tr("Pause clipboard");
    m_clipboardStatusIcon =
        qml::imageSourceFor(ImageURL::builtin(BuiltinIcon::PauseFilled).setFill(SemanticColor::Accent));
  } else {
    m_clipboardStatusText = tr("Resume clipboard");
    m_clipboardStatusIcon =
        qml::imageSourceFor(ImageURL::builtin(BuiltinIcon::PlayFilled).setFill(SemanticColor::Green));
  }
  emit clipboardStatusChanged();
}

void ClipboardHistoryViewHost::handleDataRetrieved(int totalCount) {
  m_itemCountText = tr("%n Items", nullptr, totalCount);
  emit itemCountTextChanged();
}

void ClipboardHistoryViewHost::loadDetail(const ClipboardHistoryEntry &entry) {
  unwatchDetailFile();
  m_detailTextContent.clear();
  m_detailImageSource.clear();
  m_hasDetailError = false;
  m_detailErrorTitle.clear();
  m_detailErrorDescription.clear();

  m_detailType = kindLabel(entry.kind);
  m_detailSize = formatSize(entry.size);
  m_detailCopiedAt = QDateTime::fromSecsSinceEpoch(entry.updatedAt).toString();
  m_detailMd5 = entry.md5sum;
  m_detailEntryId = entry.id;

  if (entry.encryption != ClipboardEncryptionType::None) {
    m_detailEncryptionIcon =
        qml::imageSourceFor(ImageURL::builtin(BuiltinIcon::Key).setFill(SemanticColor::Green));
  } else {
    m_detailEncryptionIcon.clear();
  }

  auto data = m_clipman->getMainOfferData(entry.id);
  if (!data) {
    m_hasDetailError = true;
    switch (data.error()) {
    case ClipboardService::OfferDecryptionError::DecryptionFailed:
      m_detailErrorTitle = tr("Decryption failed");
      m_detailErrorDescription =
          tr("Vicinae could not decrypt the data for this selection. It was most likely encrypted "
             "with a different key and cannot be recovered. You can remove this entry from the "
             "history.");
      break;
    case ClipboardService::OfferDecryptionError::DataUnavailable:
      m_detailErrorTitle = tr("Data unavailable");
      m_detailErrorDescription = tr("The data for this selection could not be found on disk.");
      break;
    case ClipboardService::OfferDecryptionError::DecryptionRequired:
      m_detailErrorTitle = tr("Data is encrypted");
      m_detailErrorDescription =
          tr("Data for this selection was previously encrypted but the clipboard is not currently "
             "configured to use encryption. You should be able to fix this by enabling it in the "
             "settings.");
      break;
    }
    m_hasDetail = true;
    emit detailChanged();
    return;
  }

  const auto &rawData = data.value();
  const auto &mime = entry.mimeType;

  if (mime == "text/uri-list") {
    QString const text(rawData);
    auto paths = text.split("\r\n", Qt::SkipEmptyParts);
    if (paths.size() == 1) {
      QUrl const url(paths.at(0));
      if (url.isLocalFile()) {
        std::error_code ec;
        std::filesystem::path const path = url.toLocalFile().toStdString();
        if (std::filesystem::is_regular_file(path, ec)) {
          auto preview = qml::resolveFilePreview(path, m_mimeDb);
          watchDetailFile(path);
          m_detailImageSource = preview.imageSource;
          m_detailTextContent = preview.textContent;
          m_hasDetail = true;
          emit detailChanged();
          return;
        }
      }
    }
  }

  if (mime.startsWith("image/")) {
    auto const cacheDir = QString::fromStdString(Omnicast::cacheDir().string());
    QDir().mkpath(cacheDir);
    QString const path = cacheDir + QStringLiteral("/clipboard-") + entry.md5sum;
    QFile f(path);
    if (!f.exists() && f.open(QIODevice::WriteOnly)) {
      f.write(rawData);
      f.close();
    }
    m_detailImageSource = qml::imageSourceFor(ImageURL::local(path));
    m_hasDetail = true;
    emit detailChanged();
    return;
  }

  if (Utils::isTextMimeType(mime) || mime == "text/uri-list") {
    static constexpr qsizetype MAX_DISPLAY = 10 * 1024;
    m_detailTextContent = QString::fromUtf8(rawData.first(qMin<qsizetype>(rawData.size(), MAX_DISPLAY)));
    m_hasDetail = true;
    emit detailChanged();
    return;
  }

  m_hasDetail = true;
  emit detailChanged();
}

void ClipboardHistoryViewHost::reloadDetail() {
  const auto entries = m_section.entries();
  const auto it = std::ranges::find(entries, m_detailEntryId, &ClipboardHistoryEntry::id);

  if (it == entries.end()) return;

  // an image file keeps the same source, it has to change for the image to be loaded again
  if (!m_detailImageSource.isEmpty()) {
    m_detailImageSource.clear();
    emit detailChanged();
  }

  loadDetail(*it);
}

void ClipboardHistoryViewHost::watchDetailFile(const std::filesystem::path &path) {
  m_detailFile = QString::fromStdString(path.string());
  m_detailFileModifiedAt = QFileInfo(m_detailFile).lastModified();
  // the directory is watched as well, as saving by renaming a new file over the old one drops the file watch
  m_detailFileWatcher.addPaths({m_detailFile, QString::fromStdString(path.parent_path().string())});
}

void ClipboardHistoryViewHost::unwatchDetailFile() {
  m_detailFileDebounce.stop();
  m_detailFile.clear();

  if (const auto paths = m_detailFileWatcher.files() + m_detailFileWatcher.directories(); !paths.isEmpty()) {
    m_detailFileWatcher.removePaths(paths);
  }
}

void ClipboardHistoryViewHost::reloadChangedDetailFile() {
  if (m_detailFile.isEmpty() || QFileInfo(m_detailFile).lastModified() == m_detailFileModifiedAt) return;

  reloadDetail();
}

void ClipboardHistoryViewHost::clearDetail() {
  unwatchDetailFile();
  if (!m_hasDetail) return;
  m_hasDetail = false;
  m_hasDetailError = false;
  m_detailTextContent.clear();
  m_detailImageSource.clear();
  m_detailType.clear();
  m_detailSize.clear();
  m_detailCopiedAt.clear();
  m_detailMd5.clear();
  m_detailEntryId.clear();
  m_detailEncryptionIcon.clear();
  m_detailErrorTitle.clear();
  m_detailErrorDescription.clear();
  emit detailChanged();
}

void ClipboardHistoryViewHost::saveDropdownFilter(const QString &value) {
  command()->storage().setItem("filter", value);
}

std::optional<QString> ClipboardHistoryViewHost::getSavedDropdownFilter() {
  auto value = command()->storage().getItem("filter");
  if (value.isNull()) return std::nullopt;
  return value.toString();
}
