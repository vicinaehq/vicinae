#include "file-search-request-router.hpp"
#include "proto/file-search.pb.h"
#include "services/files-service/abstract-file-indexer.hpp"
#include "services/files-service/file-service.hpp"
#include <qmimedatabase.h>

namespace files = proto::ext::file_search;

PromiseLike<proto::ext::extension::Response *> FileSearchRequestRouter::route(const files::Request &req) {
  switch (req.payload_case()) {
  case files::Request::kSearch:
    return search(req.search()).then(wrapUI);
  default:
    break;
  }

  return nullptr;
}

FileSearchRequestRouter::FileSearchRequestRouter(FileService &indexer) : m_indexer(indexer) {}

QFuture<files::Response *> FileSearchRequestRouter::search(const files::SearchRequest &req) {
  auto query = m_indexer.queryAsync(req.query());

  return query.then([](const std::vector<IndexerFileResult> &results) {
    QMimeDatabase mimeDb; // TODO: get rid of this when we have the mime type included in the results
    auto res = new files::Response;
    auto searchRes = new proto::ext::file_search::SearchResponse;

    res->set_allocated_search(searchRes);

    for (const auto &file : results) {
      auto mime = mimeDb.mimeTypeForFile(file.path.c_str());

      if (!mime.isValid()) continue;

      auto info = searchRes->add_files();

      info->set_path(file.path);
      info->set_mime_type(mime.name().toStdString());
    }

    return res;
  });
}

proto::ext::extension::Response *FileSearchRequestRouter::wrapUI(files::Response *sfRes) {
  auto res = new proto::ext::extension::Response;
  auto data = new proto::ext::extension::ResponseData;

  data->set_allocated_file_search(sfRes);
  res->set_allocated_data(data);
  return res;
};
