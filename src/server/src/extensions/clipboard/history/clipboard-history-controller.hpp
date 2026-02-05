#pragma once
#include "services/clipboard/clipboard-service.hpp"
#include "services/clipboard/clipboard-db.hpp"
#include <qfuturewatcher.h>
#include <qobject.h>
#include <optional>

class ClipboardHistoryModel;

class ClipboardHistoryController : public QObject {
  Q_OBJECT

  using QueryWatcher = QFutureWatcher<PaginatedResponse<ClipboardHistoryEntry>>;

public:
  static constexpr int DEFAULT_PAGE_SIZE = 1000;

  ClipboardHistoryController(ClipboardService *clipboard, ClipboardHistoryModel *model,
                             QObject *parent = nullptr);

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
  ClipboardHistoryModel *m_model = nullptr;
  ClipboardService *m_clipboard = nullptr;

  QueryWatcher m_watcher;
  QString m_query;
  std::optional<ClipboardOfferKind> m_kind;
};
