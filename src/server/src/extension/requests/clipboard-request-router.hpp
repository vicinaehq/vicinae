#pragma once
#include "proto/clipboard.pb.h"
#include "proto/extension.pb.h"
#include "services/clipboard/clipboard-service.hpp"
#include <qobject.h>

class ClipboardRequestRouter : public QObject {
  ClipboardService &m_clipboard;

  Clipboard::Content parseProtoClipboardContent(const proto::ext::clipboard::ClipboardContent &content);

  proto::ext::clipboard::Response *copy(const proto::ext::clipboard::CopyToClipboardRequest &req);
  proto::ext::clipboard::Response *paste(const proto::ext::clipboard::PasteToClipboardRequest &req);
  proto::ext::clipboard::Response *readContent(const proto::ext::clipboard::ReadContentRequest &req);
  proto::ext::clipboard::Response *clear(const proto::ext::clipboard::ClearRequest &req);

public:
  proto::ext::extension::Response *route(const proto::ext::clipboard::Request &req);
  ClipboardRequestRouter(ClipboardService &clipboard);
};
