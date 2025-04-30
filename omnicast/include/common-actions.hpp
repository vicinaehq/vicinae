#pragma once
#include "clipboard/clipboard-service.hpp"
#include "omni-icon.hpp"
#include "app.hpp"
#include "service-registry.hpp"
#include "ui/action-pannel/action.hpp"

class PasteAction : public AbstractAction {
  Clipboard::Content m_content;

  void execute(AppWindow &app) override {
    auto wm = ServiceRegistry::instance()->windowManager();
    auto clipman = ServiceRegistry::instance()->clipman();

    clipman->copyContent(m_content);
    app.hide();
    QTimer::singleShot(10, [wm]() { wm->pasteToFocusedWindow(); });
  }

public:
  PasteAction(const Clipboard::Content &content)
      : AbstractAction("Paste content to app", BuiltinOmniIconUrl("clipboard")), m_content(content) {}
};
