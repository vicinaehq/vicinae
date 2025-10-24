#include "clipboard-request-router.hpp"
#include "environment.hpp"
#include "extension/extension-navigation-controller.hpp"
#include "proto/clipboard.pb.h"
#include "services/app-service/app-service.hpp"
#include "services/clipboard/clipboard-service.hpp"
#include "services/window-manager/window-manager.hpp"
#include <qclipboard.h>

namespace clip_proto = proto::ext::clipboard;

ClipboardRequestRouter::ClipboardRequestRouter(ClipboardService &clipboard, WindowManager &wm,
                                               const AppService &appDb, ExtensionNavigationController &nav)
    : m_clipboard(clipboard), m_wm(wm), m_appDb(appDb), m_nav(nav) {}

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

  ensureCopy(content, Clipboard::CopyOptions{}, [this]() {
    QTimer::singleShot(Environment::pasteDelay(),
                       [wm = &m_wm, &app = m_appDb]() { wm->pasteToFocusedWindow(app); });
  });

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

void ClipboardRequestRouter::ensureCopy(const Clipboard::Content &content,
                                        const Clipboard::CopyOptions options,
                                        const std::function<void(void)> &cb) {
  qDebug() << "ensure copy";
  // if window is not opened we need to show it for copy time, as some wayland compositors such as niri
  // do seem to have a hard time with copying in the background in some circumstances.

  bool closeAborted = false;

  if (m_nav.handle()->isScheduledToClose()) {
    qDebug() << "aborting scheduled close";
    m_nav.handle()->abortScheduledClose();
    closeAborted = true;
  }

  if (!m_nav.handle()->isWindowOpened()) {
    m_nav.handle()->showWindow();
    closeAborted = true;
    QTimer::singleShot(Environment::pasteDelay(), this, [this, cb, content, options]() {
      m_clipboard.copyContent(content, options);
      m_nav.handle()->closeWindow();
      if (cb) { cb(); };
    });
  } else {
    m_clipboard.copyContent(content, options);
    if (cb) cb();
  }

  if (closeAborted) m_nav.handle()->closeWindow();
}
