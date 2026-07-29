#include "clipboard-history-controller.hpp"

ClipboardHistoryController::ClipboardHistoryController(ClipboardService *clipboard, QObject *parent)
    : QObject(parent), m_clipboard(clipboard) {

  m_debounce.setSingleShot(true);
  m_debounce.setInterval(100);
  connect(&m_debounce, &QTimer::timeout, this, &ClipboardHistoryController::runQuery);

  connect(&m_watcher, &QueryWatcher::finished, this, &ClipboardHistoryController::handleResults);
  connect(clipboard, &ClipboardService::selectionPinStatusChanged, this,
          &ClipboardHistoryController::handleClipboardChanged);
  connect(clipboard, &ClipboardService::selectionRemoved, this,
          &ClipboardHistoryController::handleClipboardChanged);
  connect(clipboard, &ClipboardService::allSelectionsRemoved, this,
          &ClipboardHistoryController::handleClipboardChanged);
  connect(clipboard, &ClipboardService::itemInserted, this,
          &ClipboardHistoryController::handleClipboardChanged);
  connect(clipboard, &ClipboardService::selectionUpdated, this,
          &ClipboardHistoryController::handleClipboardChanged);
}

void ClipboardHistoryController::setFilter(const QString &query) {
  m_query = query;
  m_debounce.start();
}

void ClipboardHistoryController::runQuery() {
  emit dataLoadingChanged(true);
  m_watcher.setFuture(m_clipboard->listAll(DEFAULT_PAGE_SIZE, 0, {.query = m_query, .kind = m_kind}));
}

void ClipboardHistoryController::setKindFilter(std::optional<ClipboardOfferKind> kind) {
  m_kind = kind;
  reloadSearch();
}

void ClipboardHistoryController::reloadSearch() {
  m_debounce.stop();
  runQuery();
}

void ClipboardHistoryController::handleResults() {
  if (!m_watcher.isFinished()) return;
  emit dataLoadingChanged(false);
  auto res = m_watcher.result();
  emit dataRetrieved(res);
}

void ClipboardHistoryController::handleClipboardChanged() { reloadSearch(); }
