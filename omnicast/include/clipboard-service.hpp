#pragma once
#include "common.hpp"
#include <QClipboard>
#include <QSqlError>
#include <cmath>
#include <cstdint>
#include <qapplication.h>
#include <qclipboard.h>
#include <qcryptographichash.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qlogging.h>
#include <qmimedatabase.h>
#include <qobject.h>
#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qstringview.h>
#include <qtmetamacros.h>
#include "clipboard-manager.hpp"

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

class ClipboardService : public QObject, public NonAssignable {
  Q_OBJECT
  QSqlDatabase db;
  QMimeDatabase _mimeDb;
  QFileInfo _path;
  QDir _data_dir;

public:
  ClipboardService(const QString &path);

  bool setPinned(int id, bool pinned);
  PaginatedResponse<ClipboardHistoryEntry> listAll(int limit = 100, int offset = 0) const;
  bool indexTextDocument(int id, const QByteArray &buf);
  std::vector<ClipboardHistoryEntry> collectedSearch(const QString &q);
  bool copy(const QByteArray &data);
  int insertHistoryLine(const InsertClipboardHistoryLine &payload);
  void copyText(const QString &text);
  void saveSelection(const ClipboardSelection &selection);

signals:
  void itemCopied(const InsertClipboardHistoryLine &item) const;
};
