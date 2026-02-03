#pragma once
#include "services/files-service/abstract-file-indexer.hpp"
#include "ui/views/typed-list-view.hpp"
#include "file-search-model.hpp"
#include <qfuturewatcher.h>

class SearchFilesView : public TypedListView<FileSearchModel> {
public:
  using Watcher = QFutureWatcher<std::vector<IndexerFileResult>>;

  SearchFilesView();

protected:
  std::unique_ptr<ActionPanelState> createActionPanel(const ItemType &item) const override;
  QWidget *generateDetail(const ItemType &item) const override;

private:
  void initialize() override;
  void handleSearchResults();
  void generateFilteredList(const QString &query);
  void renderRecentFiles();
  void textChanged(const QString &query) override;
  void handleDebounce();

  QTimer m_debounce;
  Watcher m_pendingFileResults;
  QString m_lastSearchText;
  QString currentQuery;
  FileSearchModel *m_model = nullptr;
};
