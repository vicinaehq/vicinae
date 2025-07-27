#pragma once
#include "common.hpp"
#include "services/clipboard/clipboard-service.hpp"
#include "omni-icon.hpp"
#include "service-registry.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/keyboard.hpp"
#include <qdnslookup.h>

class CopyToClipboardAction : public AbstractAction {
  Clipboard::Content m_content;
  Clipboard::CopyOptions m_opts;

public:
  void execute(ApplicationContext *ctx) override {
    auto clipman = ctx->services->clipman();

    if (clipman->copyContent(m_content, m_opts)) {
      ctx->navigation->showHud("Copied to clipboard", BuiltinOmniIconUrl("copy-clipboard"));
      return;
    }
  }

public:
  CopyToClipboardAction(const Clipboard::Content &content, const QString &title = "Copy to clipboard",
                        const Clipboard::CopyOptions options = {})
      : AbstractAction(title, BuiltinOmniIconUrl("copy-clipboard")), m_content(content), m_opts(options) {}
};

class PasteToFocusedWindowAction : public AbstractAction {
  Clipboard::Content m_content;

  QString title() const override {
    auto wm = ServiceRegistry::instance()->windowManager();

    if (!wm->ping()) return _title;

    auto appDb = ServiceRegistry::instance()->appDb();
    auto window = wm->getActiveWindowSync();
    QString name;

    if (auto app = appDb->find(window->wmClass())) {
      name = QString("Paste to %1").arg(app->name());
    } else {
      name = QString("Paste to %1").arg(window->title());
    }

    return name;
  }

protected:
  void execute(ApplicationContext *ctx) override {
    auto wm = ctx->services->windowManager();
    auto clipman = ctx->services->clipman();
    auto appDb = ctx->services->appDb();
    auto window = wm->getActiveWindowSync();
    KeyboardShortcut shortcut = KeyboardShortcut::paste();

    if (auto app = appDb->find(window->wmClass())) {
      if (app->isTerminalEmulator()) { shortcut = KeyboardShortcut::shiftPaste(); }
    }

    ctx->navigation->closeWindow();
    clipman->copyContent(m_content, {.concealed = true});
    QTimer::singleShot(10, [wm, window, shortcut]() { wm->sendShortcutSync(*window.get(), shortcut); });
  }

  void loadClipboardData(const Clipboard::Content &content) { m_content = content; }

public:
  PasteToFocusedWindowAction(const Clipboard::Content &content = Clipboard::NoData{})
      : AbstractAction("Copy to focused window", BuiltinOmniIconUrl("copy-clipboard")), m_content(content) {}
};
