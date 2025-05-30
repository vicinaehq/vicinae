#pragma once
#include "common.hpp"
#include <QClipboard>
#include <QSqlError>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <qapplication.h>
#include <qclipboard.h>
#include <qcryptographichash.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qjsonobject.h>
#include <qjsonvalue.h>
#include <qlogging.h>
#include <qmimedata.h>
#include <qmimedatabase.h>
#include <qobject.h>
#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qstringview.h>
#include <qtmetamacros.h>
#include <variant>
#include "services/clipboard/clipboard-server.hpp"

namespace Clipboard {
static const char *CONCEALED_MIME_TYPE = "omnicast/concealed";
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

class ClipboardService : public QObject, public NonAssignable {
  Q_OBJECT

  // prepared statements
  QSqlQuery m_retrieveSelectionByIdQuery;

  // end prepare statements

  QSqlDatabase db;
  QMimeDatabase _mimeDb;
  QFileInfo _path;
  QDir _data_dir;
  std::unique_ptr<AbstractClipboardServer> m_clipboardServer;

  std::string getSelectionPreferredMimeType(const ClipboardSelection &selection) const;
  QString createTextPreview(const QByteArray &data, int maxLength = 50) const;

public:
  ClipboardService(const std::filesystem::path &path);

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
  bool clear();
  void saveSelection(const ClipboardSelection &selection);
  ClipboardSelection retrieveSelection(int offset = 0);
  std::optional<ClipboardSelection> retrieveSelectionById(int id);
  bool copySelection(const ClipboardSelection &selection, const Clipboard::CopyOptions &options);
  bool copyQMimeData(QMimeData *data, const Clipboard::CopyOptions &options = {});

signals:
  void itemCopied(const InsertClipboardHistoryLine &item) const;
  void itemInserted(const ClipboardHistoryEntry &entry) const;
};
