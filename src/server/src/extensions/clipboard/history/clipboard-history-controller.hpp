#pragma once
#include "services/clipboard/clipboard-service.hpp"
#include "services/clipboard/clipboard-db.hpp"
#include <QTimer>
#include <qfuturewatcher.h>
#include <qobject.h>
#include <optional>

class ClipboardHistoryController : public QObject {
  Q_OBJECT

  using QueryWatcher = QFutureWatcher<PaginatedResponse<ClipboardHistoryEntry>>;

public:
  static constexpr int DEFAULT_PAGE_SIZE = 1000;

  ClipboardHistoryController(ClipboardService *clipboard, QObject *parent = nullptr);

  void setFilter(const QString &query);
  void setKindFilter(std::optional<ClipboardOfferKind> kind);
  void reloadSearch();

signals:
  void dataLoadingChanged(bool value);
  void dataRetrieved(const PaginatedResponse<ClipboardHistoryEntry> &res);

private slots:
  void handleResults();
  void handleClipboardChanged();

private:
  void runQuery();

  ClipboardService *m_clipboard = nullptr;

  QueryWatcher m_watcher;
  QTimer m_debounce;
  QString m_query;
  std::optional<ClipboardOfferKind> m_kind;
};
