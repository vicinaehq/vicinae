#include "clipboard-request-router.hpp"
#include "proto/clipboard.pb.h"
#include <qclipboard.h>

namespace clip_proto = proto::ext::clipboard;

Clipboard::Content
ClipboardRequestRouter::parseProtoClipboardContent(const clip_proto::ClipboardContent &content) {
  using ProtoContent = clip_proto::ClipboardContent;

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

clip_proto::Response *ClipboardRequestRouter::readContent(const clip_proto::ReadContentRequest &req) {
  auto readContent = m_clipboard.readContent();
  auto res = new clip_proto::Response;
  auto data = new clip_proto::ReadContentResponse;
  auto content = new clip_proto::ClipboardReadContent();

  content->set_text(readContent.text.toStdString());

  if (auto &html = readContent.html) { content->set_html(html->toStdString()); }
  if (auto &file = readContent.file) { content->set_file(file->toStdString()); }

  data->set_allocated_content(content);
  res->set_allocated_read_content(data);

  return res;
}

clip_proto::Response *ClipboardRequestRouter::clear(const clip_proto::ClearRequest &req) {
  auto res = new clip_proto::Response;

  QApplication::clipboard()->clear();
  res->set_allocated_clear(new clip_proto::ClearResponse());

  return res;
}

clip_proto::Response *ClipboardRequestRouter::paste(const clip_proto::PasteToClipboardRequest &req) {
  auto content = parseProtoClipboardContent(req.content());

  QTimer::singleShot(100, [&clip = m_clipboard, content]() { clip.pasteContent(content); });

  auto resData = new clip_proto::PasteToClipboardResponse;
  auto res = new clip_proto::Response;

  res->set_allocated_paste(resData);

  return res;
}

clip_proto::Response *ClipboardRequestRouter::copy(const clip_proto::CopyToClipboardRequest &req) {
  auto content = parseProtoClipboardContent(req.content());
  bool concealed = req.options().concealed();

  m_clipboard.copyContent(content, {.concealed = concealed});

  auto resData = new clip_proto::CopyToClipboardResponse;
  auto res = new clip_proto::Response;

  res->set_allocated_copy(resData);

  return res;
}

proto::ext::extension::Response *ClipboardRequestRouter::route(const clip_proto::Request &req) {
  namespace clipboard = clip_proto;

  auto wrap = [](clip_proto::Response *clipRes) -> proto::ext::extension::Response * {
    auto res = new proto::ext::extension::Response;
    auto data = new proto::ext::extension::ResponseData;

    data->set_allocated_clipboard(clipRes);
    res->set_allocated_data(data);
    return res;
  };

  switch (req.payload_case()) {
  case clipboard::Request::kCopy:
    return wrap(copy(req.copy()));
  case clipboard::Request::kPaste:
    return wrap(paste(req.paste()));
  case clipboard::Request::kReadContent:
    return wrap(readContent(req.read_content()));
  case clipboard::Request::kClear:
    return wrap(clear(req.clear()));
  default:
    break;
  }

  return nullptr;
}

ClipboardRequestRouter::ClipboardRequestRouter(ClipboardService &clipboard) : m_clipboard(clipboard) {}
