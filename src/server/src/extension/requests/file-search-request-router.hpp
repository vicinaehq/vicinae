#include "proto/extension.pb.h"
#include "proto/file-search.pb.h"
#include "services/files-service/file-service.hpp"
#include "types.hpp"
#include <qobject.h>

class FileSearchRequestRouter : public QObject {
public:
  PromiseLike<proto::ext::extension::Response *> route(const proto::ext::file_search::Request &req);
  FileSearchRequestRouter(FileService &fileService);

private:
  QFuture<proto::ext::file_search::Response *> search(const proto::ext::file_search::SearchRequest &req);
  static proto::ext::extension::Response *wrapUI(proto::ext::file_search::Response *fileSearch);

  FileService &m_indexer;
};
