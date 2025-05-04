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
      return;
    }

    app.statusBar->setToast("Failed to copy to clipboard", ToastPriority::Danger);
  }

public:
  CopyToClipboardAction(const Clipboard::Content &content, const QString &title = "Copy to clipboard",
                        const Clipboard::CopyOptions options = {})
      : AbstractAction(title, BuiltinOmniIconUrl("clipboard-copy")), m_content(content), m_opts(options) {}
};
