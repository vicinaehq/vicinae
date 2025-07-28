#include "clipboard-request-router.hpp"
#include "proto/clipboard.pb.h"

Clipboard::Content
ClipboardRequestRouter::parseProtoClipboardContent(const proto::ext::clipboard::ClipboardContent &content) {
  using ProtoContent = proto::ext::clipboard::ClipboardContent;

  switch (content.content_case()) {
  case ProtoContent::kText:
    return Clipboard::Text(content.text().c_str());
  case ProtoContent::kHtml: {
    auto &html = content.html();

    return Clipboard::Html({html.html().c_str(), html.text().c_str()});
  }
  case ProtoContent::kPath:
    return Clipboard::File(content.path().path());
  default:
    break;
  }

  return {};
}

proto::ext::clipboard::Response *
ClipboardRequestRouter::copy(const proto::ext::clipboard::CopyToClipboardRequest &req) {
  auto content = parseProtoClipboardContent(req.content());
  bool concealed = req.options().concealed();

  m_clipboard.copyContent(content, {.concealed = concealed});

  auto resData = new proto::ext::clipboard::CopyToClipboardResponse;
  auto res = new proto::ext::clipboard::Response;

  res->set_allocated_copy(resData);

  return res;
}

proto::ext::extension::Response *ClipboardRequestRouter::route(const proto::ext::clipboard::Request &req) {
  namespace clipboard = proto::ext::clipboard;

  auto wrap = [](proto::ext::clipboard::Response *clipRes) -> proto::ext::extension::Response * {
    auto res = new proto::ext::extension::Response;
    auto data = new proto::ext::extension::ResponseData;

    data->set_allocated_clipboard(clipRes);
    res->set_allocated_data(data);
    return res;
  };

  switch (req.payload_case()) {
  case clipboard::Request::kCopy:
    return wrap(copy(req.copy()));
  default:
    break;
  }

  return nullptr;
}

ClipboardRequestRouter::ClipboardRequestRouter(ClipboardService &clipboard) : m_clipboard(clipboard) {}
