#pragma once
#include "proto/clipboard.pb.h"
#include "proto/extension.pb.h"
#include "services/clipboard/clipboard-service.hpp"

class ClipboardRequestRouter {
  ClipboardService &m_clipboard;

  Clipboard::Content parseProtoClipboardContent(const proto::ext::clipboard::ClipboardContent &content);

  proto::ext::clipboard::Response *copy(const proto::ext::clipboard::CopyToClipboardRequest &req);

public:
  proto::ext::extension::Response *route(const proto::ext::clipboard::Request &req);
  ClipboardRequestRouter(ClipboardService &clipboard);
};
