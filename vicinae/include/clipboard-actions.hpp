#pragma once
#include "common.hpp"
#include "services/clipboard/clipboard-service.hpp"
#include "../src/ui/image/url.hpp"
#include "services/window-manager/window-manager.hpp"
#include "navigation-controller.hpp"
#include "services/app-service/app-service.hpp"
#include <QTimer>
#include "service-registry.hpp"
#include "ui/action-pannel/action.hpp"
#include <qdnslookup.h>

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
      : AbstractAction(title, ImageURL::builtin("copy-clipboard")), m_content(content), m_opts(options) {}
};

class PasteToFocusedWindowAction : public AbstractAction {

public:
  void setConcealed(bool value = true) { m_concealed = value; }

  QString title() const override {
    auto wm = ServiceRegistry::instance()->windowManager();

    if (!wm->provider()->ping()) return m_title;

    auto appDb = ServiceRegistry::instance()->appDb();
    auto window = wm->getFocusedWindow();

    if (window) {
      if (auto app = appDb->find(window->wmClass())) {
        return QString("Paste to %1").arg(app->displayName());
      }
      return QString("Paste to %1").arg(window->title());
    }

    return "Paste entry";
  }

  PasteToFocusedWindowAction(const Clipboard::Content &content = Clipboard::NoData{})
      : AbstractAction("Copy to focused window", ImageURL::builtin("copy-clipboard")), m_content(content) {}

protected:
  void execute(ApplicationContext *ctx) override {
    auto clipman = ctx->services->clipman();
    ctx->navigation->closeWindow();
    clipman->pasteContent(m_content, {.concealed = m_concealed});
  }

  void loadClipboardData(const Clipboard::Content &content) { m_content = content; }

private:
  Clipboard::Content m_content;
  bool m_concealed = false;
};
