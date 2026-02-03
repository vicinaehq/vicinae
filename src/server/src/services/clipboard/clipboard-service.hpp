#pragma once
#include "common.hpp"
#include "extensions/wm/wm-extension.hpp"
#include "services/app-service/app-service.hpp"
#include "services/clipboard/clipboard-db.hpp"
#include "services/clipboard/clipboard-encrypter.hpp"
#include "services/clipboard/clipboard-server.hpp"
#include "services/window-manager/abstract-window-manager.hpp"
#include "services/window-manager/window-manager.hpp"
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

struct SelectionRecordHandle {
  QString id;
};

struct CopyOptions {
  bool concealed = false;
};

using Content = std::variant<NoData, File, Text, Html, SelectionRecordHandle, ClipboardSelection>;

struct ReadContent {
  QString text;
  std::optional<QString> html;
  std::optional<QString> file;
};

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

class ClipboardService : public QObject, public NonCopyable {
  Q_OBJECT

signals:
  void allSelectionsRemoved() const;
  void itemCopied(const InsertClipboardHistoryLine &item) const;
  void itemInserted(const ClipboardHistoryEntry &entry) const;
  void selectionPinStatusChanged(const QString &id, bool pinned) const;
  void selectionRemoved(const QString &id) const;
  /**
   * When a selection is copied, its update time is modified which makes it appear on top
   * of the list.
   */
  void selectionUpdated() const;
  void monitoringChanged(bool value) const;

public:
  enum class OfferDecryptionError {
    DecryptionRequired, // if encryption is disabled and data was previous encrypted
    DecryptionFailed,
  };

  ClipboardService(const std::filesystem::path &path, WindowManager &wm, AppService &appDb);

  static QString readText();
  static Clipboard::ReadContent readContent();

  bool removeAllSelections();

  std::optional<QString> retrieveKeywords(const QString &id);
  bool setKeywords(const QString &id, const QString &keywords);

  std::expected<QByteArray, OfferDecryptionError> getMainOfferData(const QString &selectionId) const;
  AbstractClipboardServer *clipboardServer() const;
  bool removeSelection(const QString &id);
  bool setPinned(const QString id, bool pinned);
  QFuture<PaginatedResponse<ClipboardHistoryEntry>> listAll(int limit = 100, int offset = 0,
                                                            const ClipboardListSettings &opts = {}) const;
  bool copyText(const QString &text, const Clipboard::CopyOptions &options = {.concealed = true});
  bool copyHtml(const Clipboard::Html &data, const Clipboard::CopyOptions &options = {.concealed = false});
  bool copyFile(const std::filesystem::path &path,
                const Clipboard::CopyOptions &options = {.concealed = false});
  bool copyContent(const Clipboard::Content &content,
                   const Clipboard::CopyOptions options = {.concealed = false});
  bool pasteContent(const Clipboard::Content &content,
                    const Clipboard::CopyOptions options = {.concealed = false});
  void setRecordAllOffers(bool value);
  bool clear();
  void saveSelection(ClipboardSelection selection);
  ClipboardSelection retrieveSelection(int offset = 0);
  std::optional<ClipboardSelection> retrieveSelectionById(const QString &id);
  bool copySelectionRecord(const QString &id, const Clipboard::CopyOptions &options);
  bool copySelection(const ClipboardSelection &selection, const Clipboard::CopyOptions &options);
  bool copyQMimeData(QMimeData *data, const Clipboard::CopyOptions &options = {});

  /**
   * Whether we have a working clipboard server implementation to use.
   * This does not take into account the current monitoring preference, only
   * whether we are able to monitor.
   */
  bool supportsMonitoring() const;
  bool monitoring() const;
  void setMonitoring(bool value);
  void setEncryption(bool value);
  void setIgnorePasswords(bool value);
  bool isEncryptionReady() const;

private:
  std::unique_ptr<ClipboardEncrypter> m_encrypter;

  QMimeDatabase _mimeDb;
  std::filesystem::path m_dataDir;
  std::unique_ptr<AbstractClipboardServer> m_clipboardServer;

  static QString getSelectionPreferredMimeType(const ClipboardSelection &selection);
  static QString getOfferTextPreview(const ClipboardDataOffer &offer);

  /**
   * Unique selection hash obtained by hashing all the data offer hashes together.
   * This is used to prevent reinserting the exact same selection multiple times.
   */
  QByteArray computeSelectionHash(const ClipboardSelection &selection) const;
  bool isClearSelection(const ClipboardSelection &selection) const;
  static bool isConcealedSelection(const ClipboardSelection &selection);
  static bool isPasswordSelection(const ClipboardSelection &selection);

  /**
   * Sanitize the passed selection by removing duplicate offers.
   * The selection is sanitized in place, no copy is made.
   */
  static ClipboardSelection &sanitizeSelection(ClipboardSelection &selection);

  std::expected<QByteArray, ClipboardService::OfferDecryptionError>
  decryptOffer(const QByteArray &data, ClipboardEncryptionType type) const;

  static ClipboardOfferKind getKind(const ClipboardDataOffer &offer);

  WindowManager &m_wm;
  AppService &m_appDb;

  bool m_recordAllOffers = true;
  bool m_monitoring = false;
  bool m_ignorePasswords = true;
};
