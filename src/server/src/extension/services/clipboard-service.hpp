#pragma once
#include "generated/tsapi.hpp"
#include "services/clipboard/clipboard-service.hpp"
#include "services/paste/paste-service.hpp"

class ExtClipboardService : public tsapi::AbstractClipboard {
  using Void = tsapi::Result<void>;

public:
  ExtClipboardService(tsapi::RpcTransport &transport, ClipboardService &clipboard, PasteService &paste)
      : AbstractClipboard(transport), m_clipboard(clipboard), m_paste(paste) {}

  Void::Future copy(const tsapi::ClipboardContent &content, const tsapi::ClipboardOptions &options) override {
    m_clipboard.copyContent(parseContent(content), {.concealed = options.concealed});
    return Void::ok();
  }

  Void::Future paste(const tsapi::ClipboardContent &content) override {
    m_paste.pasteContent(parseContent(content));
    return Void::ok();
  }

  Void::Future clear() override {
    QGuiApplication::clipboard()->clear();
    return Void::ok();
  }

  tsapi::Result<tsapi::ClipboardContent>::Future readContent() override {
    auto rc = ClipboardService::readContent();
    tsapi::ClipboardContent result;

    result.text = rc.text.toStdString();
    if (rc.html) result.html = rc.html->toStdString();
    if (rc.file) result.path = rc.file->toStdString();

    return tsapi::Result<tsapi::ClipboardContent>::ok(std::move(result));
  }

private:
  static Clipboard::Content parseContent(const tsapi::ClipboardContent &content) {
    if (content.html) return Clipboard::Html({QString::fromStdString(*content.html)});
    if (content.path) return Clipboard::File(std::string{*content.path});
    if (content.text) return Clipboard::Text(QString::fromStdString(*content.text));
    return {};
  }

  ClipboardService &m_clipboard;
  PasteService &m_paste;
};
