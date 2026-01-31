#include "clipboard-history-controller.hpp"
#include "clipboard-history-model.hpp"

ClipboardHistoryController::ClipboardHistoryController(ClipboardService *clipboard,
                                                       ClipboardHistoryModel *model, QObject *parent)
    : QObject(parent), m_model(model), m_clipboard(clipboard) {

  connect(&m_watcher, &QueryWatcher::finished, this, &ClipboardHistoryController::handleResults);
  connect(&m_countWatcher, &CountWatcher::finished, this, &ClipboardHistoryController::handleCountResults);
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
  if (m_watcher.isRunning()) {
    m_watcher.cancel();
    m_watcher.waitForFinished();
  }
  if (m_countWatcher.isRunning()) {
    m_countWatcher.cancel();
    m_countWatcher.waitForFinished();
  }
  emit dataLoadingChanged(true);

  ClipboardListSettings opts{.query = query, .kind = m_kind};

  m_watcher.setFuture(m_clipboard->listAll(DEFAULT_PAGE_SIZE, 0, opts));

  m_countWatcher.setFuture(m_clipboard->countAll(opts));
}

void ClipboardHistoryController::setKindFilter(std::optional<ClipboardOfferKind> kind) {
  m_kind = kind;
  setFilter(m_query);
}

void ClipboardHistoryController::reloadSearch() { setFilter(m_query); }

void ClipboardHistoryController::handleResults() {
  if (!m_watcher.isFinished()) return;
  emit dataLoadingChanged(false);
  auto res = m_watcher.result();
  m_model->setData(res);
  emit dataRetrieved(res);
}

void ClipboardHistoryController::handleCountResults() {
  if (!m_countWatcher.isFinished()) return;
  emit countRetrieved(m_countWatcher.result());
}

void ClipboardHistoryController::handleClipboardChanged() { reloadSearch(); }
