#pragma once
#include <QCoreApplication>
#include <QTimer>
#include "builtin_icon.hpp"
#include "services/clipboard/clipboard-service.hpp"
#include "services/paste/paste-service.hpp"
#include "ui/image/url.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/app-runtime/app-runtime.hpp"
#include "services/app-service/app-service.hpp"
#include "services/window-manager/window-manager.hpp"
#include "ui/action-pannel/action.hpp"

class CopyToClipboardAction : public AbstractAction {
  Q_DECLARE_TR_FUNCTIONS(CopyToClipboardAction)

public:
  void execute(ApplicationContext *ctx) override {
    auto clipman = ctx->services->clipman();

    if (clipman->copyContent(std::move(m_content), m_opts)) {
      ctx->navigation->showHud(tr("Copied to clipboard"), ImageURL::builtin(BuiltinIcon::CopyClipboard));
      return;
    }
  }

  CopyToClipboardAction(Clipboard::Content content, const QString &title = tr("Copy to clipboard"),
                        const Clipboard::CopyOptions &options = {})
      : AbstractAction(title, BuiltinIcon::CopyClipboard), m_content(std::move(content)), m_opts(options) {}

private:
  Clipboard::Content m_content;
  Clipboard::CopyOptions m_opts;
};

class PasteToFocusedWindowAction : public AbstractAction {
  Q_DECLARE_TR_FUNCTIONS(PasteToFocusedWindowAction)

public:
  void setConcealed(bool value = true) { m_concealed = value; }

  QString title() const override {
    const auto name = targetName();
    if (name.isEmpty()) return tr("Paste to active window");
    return tr("Paste to %1").arg(name);
  }

  std::optional<ImageURL> icon() const override {
    if (auto app = targetApp()) return app->iconUrl();
    return m_icon;
  }

  PasteToFocusedWindowAction(const Clipboard::Content &content = Clipboard::NoData{})
      : AbstractAction(tr("Copy to focused window"), ImageURL::builtin(BuiltinIcon::CopyClipboard)),
        m_content(content) {}

protected:
  void execute(ApplicationContext *ctx) override {
    auto paste = ctx->services->pasteService();
    paste->pasteContent(m_content, {.concealed = m_concealed});
    ctx->navigation->closeWindow();
  }

  void loadClipboardData(const Clipboard::Content &content) { m_content = content; }

private:
  static std::shared_ptr<AbstractApplication> targetApp() {
    auto *reg = ServiceRegistry::instance();
    if (!reg) return nullptr;

    if (auto *runtime = reg->appRuntime()) {
      if (auto app = runtime->pasteTargetApp()) return app;
    }

    if (auto *wm = reg->windowManager()) {
      if (auto win = wm->pasteTargetWindow()) {
        if (auto *apps = reg->appDb()) {
          auto app = apps->findByClass(win->wmClass());
          if (!app) app = apps->findById(win->wmClass());
          return app;
        }
      }
    }

    return nullptr;
  }

  static QString targetName() {
    if (auto app = targetApp()) return app->displayName();

    auto *reg = ServiceRegistry::instance();
    if (!reg) return {};

    if (auto *runtime = reg->appRuntime()) {
      const auto name = runtime->pasteTargetName();
      if (!name.isEmpty()) return name;
    }

    if (auto *wm = reg->windowManager()) {
      if (auto win = wm->pasteTargetWindow()) {
        if (!win->title().isEmpty()) return win->title();
        return win->wmClass();
      }
    }

    return {};
  }

  Clipboard::Content m_content;
  bool m_concealed = false;
};
