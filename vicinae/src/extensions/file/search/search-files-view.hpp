#pragma once
#include "services/files-service/abstract-file-indexer.hpp"
#include "ui/views/list-view.hpp"
#include <qfuturewatcher.h>
#include <qlocale.h>
#include <qmimedatabase.h>

class SearchFilesView : public ListView {
public:
  using Watcher = QFutureWatcher<std::vector<IndexerFileResult>>;

  SearchFilesView();

private:
  void initialize() override;
  void handleSearchResults();
  void generateFilteredList(const QString &query);
  void renderRecentFiles();
  void textChanged(const QString &query) override;

  Watcher m_pendingFileResults;
  QString m_lastSearchText;
  QString currentQuery;
};
