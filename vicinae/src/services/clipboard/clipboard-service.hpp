#pragma once
#include "common.hpp"
#include "extensions/wm/wm-extension.hpp"
#include "services/app-service/app-service.hpp"
#include "services/clipboard/clipboard-db.hpp"
#include "services/clipboard/clipboard-server.hpp"
#include "services/window-manager/abstract-window-manager.hpp"
#include "services/window-manager/window-manager.hpp"
#include <QString>
#include "utils/expected.hpp"
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
public:
  using GetLocalEncryptionKeyResponse = tl::expected<QByteArray, QKeychain::Error>;

private:
  Q_OBJECT

  WindowManager &m_wm;
  AppService &m_appDb;

  bool m_recordAllOffers = true;
  bool m_monitoring = false;
  std::optional<QByteArray> m_localEncryptionKey;
  bool m_isEncryptionReady = false;

  QMimeDatabase _mimeDb;
  std::filesystem::path m_dataDir;
  std::unique_ptr<AbstractClipboardServer> m_clipboardServer;

  static QString getSelectionPreferredMimeType(const ClipboardSelection &selection);
  static QString getOfferTextPreview(const ClipboardDataOffer &offer);

  QFuture<GetLocalEncryptionKeyResponse> getLocalEncryptionKey();

  /**
   * Unique selection hash obtained by hashing all the data offer hashes together.
   * This is used to prevent reinserting the exact same selection multiple times.
   */
  QByteArray computeSelectionHash(const ClipboardSelection &selection) const;
  bool isClearSelection(const ClipboardSelection &selection) const;
  static bool isConcealedSelection(const ClipboardSelection &selection);

  /**
   * Sanitize the passed selection by removing duplicate offers.
   * The selection is sanitized in place, no copy is made.
   */
  static ClipboardSelection &sanitizeSelection(ClipboardSelection &selection);

  QByteArray decryptOffer(const QByteArray &data, ClipboardEncryptionType enc) const;

  static ClipboardOfferKind getKind(const ClipboardDataOffer &offer);

public:
  ClipboardService(const std::filesystem::path &path, WindowManager &wm, AppService &app);

  bool removeAllSelections();

  std::optional<QString> retrieveKeywords(const QString &id);
  bool setKeywords(const QString &id, const QString &keywords);

  QByteArray decryptMainSelectionOffer(const QString &selectionId) const;
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
};
