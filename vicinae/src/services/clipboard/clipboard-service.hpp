#pragma once
#include "common.hpp"
#include "services/clipboard/clipboard-server.hpp"
#include <QString>
#include <expected>
#include <filesystem>
#include <QJsonObject>
#include <qdir.h>
#include <qfileinfo.h>
#include <qfuture.h>
#include <qjsonobject.h>
#include <qmimedatabase.h>
#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qstringview.h>
#include <qt6keychain/keychain.h>

namespace Clipboard {
static const char *CONCEALED_MIME_TYPE = "vicinae/concealed";
using NoData = std::monostate;
struct File {
  std::filesystem::path path;
};
struct Text {
  QString text;
};
struct Html {
  QString html;
  std::optional<QString> text;
};

struct CopyOptions {
  bool concealed = false;
};

using Content = std::variant<NoData, File, Text, Html, ClipboardSelection>;

static Content fromJson(const QJsonObject &obj) {
  if (obj.contains("path")) { return File{.path = obj.value("path").toString().toStdString()}; }
  if (obj.contains("html")) {
    Html html;

    html.html = obj.value("html").toString();

    if (obj.contains("text")) { html.text = obj.value("text").toString(); }

    return html;
  }
  if (obj.contains("text")) { return Text{obj.value("text").toString()}; }

  return Text{};
}

}; // namespace Clipboard

struct InsertClipboardHistoryLine {
  QString mimeType;
  QString textPreview;
  QString md5sum;
};

struct ClipboardHistoryEntry {
  int id;
  QString mimeType;
  QString textPreview;
  uint64_t pinnedAt;
  QString md5sum;
  uint64_t createdAt;
  QString filePath;
};

struct ClipboardListSettings {
  QString query;
};

class ClipboardService : public QObject, public NonCopyable {
public:
  using GetLocalEncryptionKeyResponse = std::expected<QByteArray, QKeychain::Error>;
  enum class ClipboardEncryptionType {
    None,
    Local,
  };

  QString stringifyEncryptionType(ClipboardEncryptionType type) const {
    switch (type) {
    case ClipboardEncryptionType::Local:
      return "local";
    case ClipboardEncryptionType::None:
      return "none";
    default:
      return "none";
    }
  }

  ClipboardEncryptionType parseEncryptionType(const QString &type) const {
    if (type == "local") return ClipboardEncryptionType::Local;
    return ClipboardEncryptionType::None;
  }

private:
  Q_OBJECT

  QSqlQuery m_retrieveSelectionByIdQuery;
  bool m_recordAllOffers = true;
  bool m_monitoring = true;
  std::optional<QByteArray> m_localEncryptionKey;
  bool m_isEncryptionReady = false;

  QSqlDatabase db;
  QMimeDatabase _mimeDb;
  QFileInfo _path;
  QDir _data_dir;
  std::filesystem::path m_dataDir;
  std::unique_ptr<AbstractClipboardServer> m_clipboardServer;

  std::string getSelectionPreferredMimeType(const ClipboardSelection &selection) const;
  QString createTextPreview(const QByteArray &data, int maxLength = 50) const;

  QFuture<GetLocalEncryptionKeyResponse> getLocalEncryptionKey();

  QByteArray encrypt(const QByteArray &data, const QByteArray &key) const;
  QByteArray decrypt(const QByteArray &data, const QByteArray &key) const;

public:
  ClipboardService(const std::filesystem::path &path);

  QByteArray decryptMainSelectionOffer(int selectionId) const;
  AbstractClipboardServer *clipboardServer() const;
  bool removeSelection(int id);
  bool setPinned(int id, bool pinned);
  PaginatedResponse<ClipboardHistoryEntry> listAll(int limit = 100, int offset = 0,
                                                   const ClipboardListSettings &opts = {}) const;
  bool copyText(const QString &text, const Clipboard::CopyOptions &options = {.concealed = true});
  bool copyHtml(const Clipboard::Html &data, const Clipboard::CopyOptions &options = {.concealed = false});
  bool copyFile(const std::filesystem::path &path,
                const Clipboard::CopyOptions &options = {.concealed = false});
  bool copyContent(const Clipboard::Content &content,
                   const Clipboard::CopyOptions options = {.concealed = false});
  void setRecordAllOffers(bool value);
  bool clear();
  void saveSelection(const ClipboardSelection &selection);
  void saveSelection2(const ClipboardSelection &selection);
  ClipboardSelection retrieveSelection(int offset = 0);
  std::optional<ClipboardSelection> retrieveSelectionById(int id);
  bool copySelection(const ClipboardSelection &selection, const Clipboard::CopyOptions &options);
  bool copyQMimeData(QMimeData *data, const Clipboard::CopyOptions &options = {});

  bool isServerRunning() const;
  bool monitoring() const;
  void setMonitoring(bool value);

signals:
  void itemCopied(const InsertClipboardHistoryLine &item) const;
  void itemInserted(const ClipboardHistoryEntry &entry) const;
  void monitoringChanged(bool value) const;
};
