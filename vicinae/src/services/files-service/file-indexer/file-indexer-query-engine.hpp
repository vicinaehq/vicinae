#pragma once
#include "services/files-service/abstract-file-indexer.hpp"
#include "services/files-service/file-indexer/file-indexer-db.hpp"
#include "utils.hpp"
#include <QtConcurrent/QtConcurrentRun>

// FIXME: this fires a new query and creates a database connection on each search, which can
// potentially cause issues when doing a lot of file search. Adding a slight debounce on
// search is recommended.
class FileIndexerQueryEngine {
public:
  QFuture<std::vector<IndexerFileResult>> query(std::string_view q,
                                                const AbstractFileIndexer::QueryParams &params) {
    return QtConcurrent::run([params, query = qStringFromStdView(q)]() {
      FileIndexerDatabase db;
      std::vector<std::filesystem::path> paths;
      QString finalQuery = preparePrefixSearchQuery(query);
      std::vector<IndexerFileResult> results;

      paths = db.search(finalQuery.toStdString(), params);
      results.reserve(paths.size());

      for (const auto &path : paths) {
        results.emplace_back(IndexerFileResult{.path = path});
      }

      return results;
    });
  }

private:
  static QString preparePrefixSearchQuery(QStringView query) {
    QString finalQuery;

    for (const auto &word : query.split(' ')) {
      if (!finalQuery.isEmpty()) { finalQuery += ' '; }
      finalQuery += QString("\"%1\"").arg(word);
    }

    finalQuery += '*';

    return finalQuery;
  }
};
