#include "clipboard-history-controller.hpp"

ClipboardHistoryController::ClipboardHistoryController(ClipboardService *clipboard, QObject *parent)
    : QObject(parent), m_clipboard(clipboard) {

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
  requestQuery();
}

void ClipboardHistoryController::setKindFilter(std::optional<ClipboardOfferKind> kind) {
  m_kind = kind;
  requestQuery();
}

void ClipboardHistoryController::reloadSearch() { requestQuery(); }

void ClipboardHistoryController::requestQuery() {
  if (m_queryRunning) {
    m_queryPending = true;
    return;
  }

  runQuery();
}

void ClipboardHistoryController::runQuery() {
  m_queryRunning = true;
  emit dataLoadingChanged(true);
  m_watcher.setFuture(m_clipboard->listAll(DEFAULT_PAGE_SIZE, 0, {.query = m_query, .kind = m_kind}));
}

void ClipboardHistoryController::handleResults() {
  if (!m_watcher.isFinished()) return;

  m_queryRunning = false;

  // never emit stale results: each delivery consumes the host's one-shot select-first flag
  if (m_queryPending) {
    m_queryPending = false;
    runQuery();
    return;
  }

  emit dataLoadingChanged(false);
  emit dataRetrieved(m_watcher.result());
}

void ClipboardHistoryController::handleClipboardChanged() { reloadSearch(); }
