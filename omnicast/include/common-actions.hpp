#pragma once
#include "services/clipboard/clipboard-service.hpp"
#include "omni-icon.hpp"
#include "app.hpp"
#include "service-registry.hpp"
#include "ui/action-pannel/action.hpp"
#include <qobjectdefs.h>

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

class PushViewAction : public AbstractAction {
  BaseView *m_view;
  OmniIconUrl m_icon;

  void execute() override {
    auto ui = ServiceRegistry::instance()->UI();

    ui->pushView(m_view, {.navigation = NavigationStatus{.title = title(), .iconUrl = m_icon}});
  }

public:
  PushViewAction(const QString &title, BaseView *view, const OmniIconUrl &icon)
      : AbstractAction(title, icon), m_view(view), m_icon(icon) {}
};
