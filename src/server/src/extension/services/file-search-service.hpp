#pragma once
#include "generated/tsapi.hpp"
#include "services/files-service/file-service.hpp"
#include <qmimedatabase.h>

class ExtFileSearchService : public tsapi::AbstractFileSearch {
public:
  ExtFileSearchService(tsapi::RpcTransport &transport, FileService &indexer)
      : AbstractFileSearch(transport), m_indexer(indexer) {}

  tsapi::Result<std::vector<tsapi::FileInfo>>::Future search(std::string q) override {
    return m_indexer.queryAsync(q).then([](const std::vector<IndexerFileResult> &results)
                                            -> tsapi::Result<std::vector<tsapi::FileInfo>>::Type {
      QMimeDatabase const mimeDb;
      std::vector<tsapi::FileInfo> out;
      out.reserve(results.size());

      for (const auto &file : results) {
        auto mime = mimeDb.mimeTypeForFile(file.path.c_str());
        if (!mime.isValid()) continue;
        out.emplace_back(tsapi::FileInfo{.path = file.path.string(), .mimeType = mime.name().toStdString()});
      }

      return out;
    });
  }

private:
  FileService &m_indexer;
};
