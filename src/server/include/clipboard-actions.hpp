#pragma once
#include <QTimer>
#include "builtin_icon.hpp"
#include "services/clipboard/clipboard-service.hpp"
#include "services/paste/paste-service.hpp"
#include "ui/image/url.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "ui/action-pannel/action.hpp"

class CopyToClipboardAction : public AbstractAction {
  Clipboard::Content m_content;
  Clipboard::CopyOptions m_opts;

public:
  void execute(ApplicationContext *ctx) override {
    auto clipman = ctx->services->clipman();

    if (clipman->copyContent(m_content, m_opts)) {
      ctx->navigation->showHud("Copied to clipboard", ImageURL::builtin("copy-clipboard"));
      return;
    }
  }

public:
  CopyToClipboardAction(const Clipboard::Content &content, const QString &title = "Copy to clipboard",
                        const Clipboard::CopyOptions options = {})
      : AbstractAction(title, BuiltinIcon::CopyClipboard), m_content(content), m_opts(options) {}
};

class PasteToFocusedWindowAction : public AbstractAction {

public:
  void setConcealed(bool value = true) { m_concealed = value; }

  QString title() const override { return "Paste to active window"; }

  PasteToFocusedWindowAction(const Clipboard::Content &content = Clipboard::NoData{})
      : AbstractAction("Copy to focused window", ImageURL::builtin("copy-clipboard")), m_content(content) {}

protected:
  void execute(ApplicationContext *ctx) override {
    auto paste = ctx->services->pasteService();
    paste->pasteContent(m_content, {.concealed = m_concealed});
    ctx->navigation->closeWindow();
  }

  void loadClipboardData(const Clipboard::Content &content) { m_content = content; }

private:
  Clipboard::Content m_content;
  bool m_concealed = false;
};
