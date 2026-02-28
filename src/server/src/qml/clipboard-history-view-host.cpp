#include "clipboard-history-view-host.hpp"
#include "extensions/clipboard/history/clipboard-history-controller.hpp"
#include "clipboard-history-model.hpp"
#include "view-utils.hpp"
#include "service-registry.hpp"
#include "services/clipboard/clipboard-service.hpp"
#include "utils/utils.hpp"
#include <QDateTime>
#include <QMimeDatabase>
#include <QUrl>

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

ClipboardHistoryViewHost::ClipboardHistoryViewHost() : ViewHostBase() {}

QUrl ClipboardHistoryViewHost::qmlComponentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/ClipboardHistoryView.qml"));
}

QUrl ClipboardHistoryViewHost::qmlSearchAccessoryUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/ClipboardFilterAccessory.qml"));
}

QVariantMap ClipboardHistoryViewHost::qmlProperties() {
  return {{QStringLiteral("host"), QVariant::fromValue(this)}};
}

void ClipboardHistoryViewHost::initialize() {
  BaseView::initialize();

  m_clipman = context()->services->clipman();
  m_model = new ClipboardHistoryModel(this);
  m_model->setScope(ViewScope(context(), this));
  m_model->initialize();
  m_controller = new ClipboardHistoryController(m_clipman, this);

  auto preferences = command()->preferenceValues();
  auto defaultActionStr = preferences.value("defaultAction").toString();
  m_model->setDefaultAction(defaultActionStr == "paste" ? ClipboardHistoryModel::DefaultAction::Paste
                                                        : ClipboardHistoryModel::DefaultAction::Copy);

  setSearchPlaceholderText("Browse clipboard history...");

  m_canToggleMonitoring = m_clipman->supportsMonitoring();
  if (!m_canToggleMonitoring) {
    m_clipboardStatusText = QStringLiteral("Clipboard monitoring unavailable");
    m_clipboardStatusIcon = qml::imageSourceFor(ImageURL::builtin("warning").setFill(SemanticColor::Red));
  } else {
    handleMonitoringChanged(m_clipman->monitoring());
  }

  connect(m_clipman, &ClipboardService::monitoringChanged, this,
          &ClipboardHistoryViewHost::handleMonitoringChanged);

  connect(m_controller, &ClipboardHistoryController::dataRetrieved, this,
          [this](const PaginatedResponse<ClipboardHistoryEntry> &page) {
            m_model->setEntries(page);
            handleDataRetrieved(page.totalCount);
          });

  connect(m_controller, &ClipboardHistoryController::dataLoadingChanged, this, &BaseView::setLoading);

  connect(m_model, &ClipboardHistoryModel::entrySelected, this, &ClipboardHistoryViewHost::loadDetail);

  auto savedFilter = getSavedDropdownFilter().value_or("all");
  if (auto it = savedFilterToKind.find(savedFilter); it != savedFilterToKind.end()) {
    m_currentKindFilter = filterIndexFromKind(it->second);
    m_controller->setKindFilter(it->second);
  }
}

void ClipboardHistoryViewHost::loadInitialData() { m_controller->setFilter(searchText()); }

void ClipboardHistoryViewHost::textChanged(const QString &text) {
  m_model->resetSelectionOnNextUpdate();
  m_controller->setFilter(text);
}

void ClipboardHistoryViewHost::onReactivated() { m_model->refreshActionPanel(); }

void ClipboardHistoryViewHost::beforePop() { m_model->beforePop(); }

QObject *ClipboardHistoryViewHost::listModel() const { return m_model; }

void ClipboardHistoryViewHost::toggleMonitoring() {
  QJsonObject patch;
  if (m_clipman->monitoring()) {
    patch["monitoring"] = false;
  } else {
    patch["monitoring"] = true;
  }
  command()->setPreferenceValues(patch);
}

void ClipboardHistoryViewHost::setKindFilter(int kind) {
  if (m_currentKindFilter == kind) return;
  m_currentKindFilter = kind;
  emit currentKindFilterChanged();

  auto offerKind = kindFromFilterIndex(kind);
  m_model->resetSelectionOnNextUpdate();
  m_controller->setKindFilter(offerKind);

  if (kind >= 0 && kind <= 4) { saveDropdownFilter(filterIndexToSavedValue[kind]); }

  if (!searchText().isEmpty()) { clearSearchText(); }
}

void ClipboardHistoryViewHost::handleMonitoringChanged(bool monitoring) {
  if (monitoring) {
    m_clipboardStatusText = QStringLiteral("Pause clipboard");
    m_clipboardStatusIcon =
        qml::imageSourceFor(ImageURL::builtin("pause-filled").setFill(SemanticColor::Orange));
  } else {
    m_clipboardStatusText = QStringLiteral("Resume clipboard");
    m_clipboardStatusIcon =
        qml::imageSourceFor(ImageURL::builtin("play-filled").setFill(SemanticColor::Green));
  }
  emit clipboardStatusChanged();
}

void ClipboardHistoryViewHost::handleDataRetrieved(int totalCount) {
  m_itemCountText = QString("%1 Items").arg(totalCount);
  emit itemCountTextChanged();
}

void ClipboardHistoryViewHost::loadDetail(const ClipboardHistoryEntry &entry) {
  m_detailTextContent.clear();
  m_detailImageSource.clear();
  m_hasDetailError = false;
  m_detailErrorTitle.clear();
  m_detailErrorDescription.clear();

  m_detailMimeType = entry.mimeType;
  m_detailSize = formatSize(entry.size);
  m_detailCopiedAt = QDateTime::fromSecsSinceEpoch(entry.updatedAt).toString();
  m_detailMd5 = entry.md5sum;

  if (entry.encryption != ClipboardEncryptionType::None) {
    m_detailEncryptionIcon = qml::imageSourceFor(ImageURL::builtin("key").setFill(SemanticColor::Green));
  } else {
    m_detailEncryptionIcon.clear();
  }

  auto data = m_clipman->getMainOfferData(entry.id);
  if (!data) {
    m_hasDetailError = true;
    switch (data.error()) {
    case ClipboardService::OfferDecryptionError::DecryptionFailed:
      m_detailErrorTitle = QStringLiteral("Decryption failed");
      m_detailErrorDescription = QStringLiteral(
          "Vicinae could not decrypt the data for this selection. This is most likely caused by a "
          "keychain software change. To fix this disable encryption in the clipboard extension "
          "settings.");
      break;
    case ClipboardService::OfferDecryptionError::DecryptionRequired:
      m_detailErrorTitle = QStringLiteral("Data is encrypted");
      m_detailErrorDescription = QStringLiteral(
          "Data for this selection was previously encrypted but the clipboard is not currently "
          "configured to use encryption. You should be able to fix this by enabling it in the "
          "clipboard extension settings.");
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
      if (url.scheme() == "file") {
        std::error_code ec;
        std::filesystem::path const path = url.path().toStdString();
        if (std::filesystem::is_regular_file(path, ec)) {
          auto fileMime = QMimeDatabase().mimeTypeForFile(path.c_str());
          if (fileMime.name().startsWith("image/")) {
            m_detailImageSource = qml::imageSourceFor(ImageURL::local(path));
            m_hasDetail = true;
            emit detailChanged();
            return;
          }
          if (Utils::isTextMimeType(fileMime)) {
            QFile file(path.c_str());
            if (file.open(QIODevice::ReadOnly)) { m_detailTextContent = QString::fromUtf8(file.readAll()); }
            m_hasDetail = true;
            emit detailChanged();
            return;
          }
        }
      }
    }
  }

  if (mime.startsWith("image/")) {
    // Create a fresh temp file each time so the URL changes and QML does not
    // serve a stale cached image from a previous selection.
    m_tmpFile = std::make_unique<QTemporaryFile>();
    m_tmpFile->setAutoRemove(true);
    if (m_tmpFile->open()) {
      m_tmpFile->write(rawData);
      m_tmpFile->flush();
      m_detailImageSource = qml::imageSourceFor(ImageURL::local(m_tmpFile->filesystemFileName()));
      m_tmpFile->close();
    }
    m_hasDetail = true;
    emit detailChanged();
    return;
  }

  if (Utils::isTextMimeType(mime) || mime == "text/uri-list") {
    m_detailTextContent = QString::fromUtf8(rawData);
    m_hasDetail = true;
    emit detailChanged();
    return;
  }

  m_hasDetail = true;
  emit detailChanged();
}

void ClipboardHistoryViewHost::clearDetail() {
  if (!m_hasDetail) return;
  m_hasDetail = false;
  m_hasDetailError = false;
  m_detailTextContent.clear();
  m_detailImageSource.clear();
  m_detailMimeType.clear();
  m_detailSize.clear();
  m_detailCopiedAt.clear();
  m_detailMd5.clear();
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
