#pragma once
#include <QFuture>
#include <vector>
#include "services/files-service/abstract-file-indexer.hpp"

/**
 * Blocks until QFuture completes.
 * Returns the result vector if successful.
 */
inline std::vector<IndexerFileResult> waitForSearchResults(QFuture<std::vector<IndexerFileResult>> future,
                                                           int timeoutMs = 5000) {
  future.waitForFinished();
  return future.result();
}
