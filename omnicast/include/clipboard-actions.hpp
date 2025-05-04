#pragma once
#include "clipboard/clipboard-service.hpp"
#include "omni-icon.hpp"
#include "service-registry.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/toast.hpp"
#include <qdnslookup.h>

class CopyToClipboardAction : public AbstractAction {
  Clipboard::Content m_content;
  Clipboard::CopyOptions m_opts;

  void execute(AppWindow &app) override {
    auto clipman = ServiceRegistry::instance()->clipman();

    if (clipman->copyContent(m_content, m_opts)) {
      app.statusBar->setToast("Copied");
      app.closeWindow();
      return;
    }

    app.statusBar->setToast("Failed to copy to clipboard", ToastPriority::Danger);
  }

public:
  CopyToClipboardAction(const Clipboard::Content &content, const QString &title = "Copy to clipboard",
                        const Clipboard::CopyOptions options = {})
      : AbstractAction(title, BuiltinOmniIconUrl("copy-clipboard")), m_content(content), m_opts(options) {}
};

class CopyToFocusedWindowAction : public AbstractAction {
  Clipboard::Content m_content;

  QString title() const override {
    auto wm = ServiceRegistry::instance()->windowManager();

    if (!wm->ping()) return _title;

    auto window = wm->getActiveWindowSync();

    return QString("Copy to %1").arg(window->title());
  }

protected:
  void execute(AppWindow &app) override {
    auto wm = ServiceRegistry::instance()->windowManager();
    auto clipman = ServiceRegistry::instance()->clipman();

    app.closeWindow();
    clipman->copyContent(m_content, {.concealed = true});
    QTimer::singleShot(10, [wm, clipman]() { wm->pasteToFocusedWindow(); });
  }

  void loadClipboardData(const Clipboard::Content &content) { m_content = content; }

public:
  CopyToFocusedWindowAction(const Clipboard::Content &content = Clipboard::NoData{})
      : AbstractAction("Copy to focused window", BuiltinOmniIconUrl("copy-clipboard")), m_content(content) {}
};
