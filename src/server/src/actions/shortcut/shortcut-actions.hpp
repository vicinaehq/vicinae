#pragma once
#include <QCoreApplication>
#include <QGuiApplication>
#include "actions/app/app-actions.hpp"
#include "builtin_icon.hpp"
#include "common.hpp"
#include "common/context.hpp"
#include "services/clipboard/clipboard-service.hpp"
#include "navigation-controller.hpp"
#include "services/shortcut/shortcut-service.hpp"
#include "qml/shortcut-form-view-host.hpp"
#include "../../ui/image/url.hpp"
#include "service-registry.hpp"
#include "ui/action-pannel/action.hpp"
#include "services/app-service/app-service.hpp"
#include "services/toast/toast-service.hpp"
#include "ui/toast/toast.hpp"
#include <memory>
#include <qclipboard.h>
#include <qlogging.h>
#include <ranges>

namespace {
QString expandShortcut(const Shortcut &sh, std::span<const QString> args) {
  QString expanded;
  size_t argumentIndex = 0;

  for (const auto &part : sh.parts()) {
    if (auto s = std::get_if<QString>(&part)) {
      expanded += *s;
    } else if (auto placeholder = std::get_if<Shortcut::ParsedPlaceholder>(&part)) {
      if (placeholder->id == "clipboard") {
        expanded += QGuiApplication::clipboard()->text();
      } else if (placeholder->id == "selected") {
        // TODO: selected text
      } else if (placeholder->id == "uuid") {
        expanded += QUuid::createUuid().toString(QUuid::StringFormat::WithoutBraces);
      } else {
        if (argumentIndex < args.size()) { expanded += args[argumentIndex++]; }
      }
    }
  }

  return expanded;
}
}; // namespace

class OpenShortcutAction : public AbstractAction {
  Q_DECLARE_TR_FUNCTIONS(OpenShortcutAction)

  std::shared_ptr<Shortcut> m_shortcut;
  std::vector<QString> m_arguments;
  std::shared_ptr<AbstractApplication> m_app;
  bool m_clearSearch = false;

public:
  void execute(ApplicationContext *ctx) override {
    auto appDb = ctx->services->appDb();
    auto toast = ctx->services->toastService();
    auto shortcut = ctx->services->shortcuts();
    const auto expanded = expandShortcut(*m_shortcut, m_arguments);

    if (m_app) {
      appDb->launch(*m_app, {expanded});
    } else if (auto app = appDb->findById(m_shortcut->app())) {
      appDb->launch(*app, {expanded});
    } else {
      toast->setToast(tr("No app with id %1").arg(m_shortcut->app()), ToastStyle::Danger);
      return;
    }

    shortcut->registerVisit(m_shortcut->id());
    ctx->navigation->closeWindow();
    if (m_clearSearch) ctx->navigation->clearSearchText();
  }

  QString title() const override { return tr("Open shortcut"); }

public:
  void setClearSearch(bool value) { m_clearSearch = value; }

  OpenShortcutAction(const std::shared_ptr<Shortcut> &shortcut, const std::vector<QString> &arguments,
                     const std::shared_ptr<AbstractApplication> &app = nullptr)
      : AbstractAction(tr("Open shortcut"), shortcut->icon()), m_shortcut(shortcut), m_arguments(arguments),
        m_app(app) {}
};

class OpenCompletedShortcutAction : public AbstractAction {
  std::shared_ptr<Shortcut> m_shortcut;
  std::shared_ptr<AbstractApplication> m_app;
  bool m_clearSearch = false;

public:
  void execute(ApplicationContext *ctx) override {
    std::vector<QString> values;

    for (const auto &val : ctx->navigation->completionValues()) {
      values.emplace_back(val.second);
    }

    OpenShortcutAction open(m_shortcut, values, m_app);

    open.setClearSearch(m_clearSearch);
    open.execute(ctx);
  }

  void setClearSearch(bool value) { m_clearSearch = value; }

  OpenCompletedShortcutAction(const std::shared_ptr<Shortcut> &shortcut,
                              const std::shared_ptr<AbstractApplication> &app = nullptr)
      : AbstractAction(QCoreApplication::translate("OpenCompletedShortcutAction", "Open shortcut"),
                       shortcut->icon()),
        m_shortcut(shortcut), m_app(app) {}
};

class OpenShortcutFromSearchText : public AbstractAction {
  std::shared_ptr<Shortcut> m_shortcut;

  void execute(ApplicationContext *ctx) override {
    OpenShortcutAction open(m_shortcut, {ctx->navigation->searchText()});

    open.execute(ctx);
  }

public:
  OpenShortcutFromSearchText(const std::shared_ptr<Shortcut> &shortcut)
      : AbstractAction(QCoreApplication::translate("OpenShortcutFromSearchText", "Open shortcut"),
                       shortcut->icon()),
        m_shortcut(shortcut) {}
};

struct EditShortcutAction : public AbstractAction {
public:
  std::shared_ptr<Shortcut> m_shortcut;

  void execute(ApplicationContext *ctx) override {
    auto view = new ShortcutFormViewHost(m_shortcut, ShortcutFormViewHost::Mode::Edit);

    ctx->navigation->pushView(view);
  }

  EditShortcutAction(const std::shared_ptr<Shortcut> &shortcut, const QList<QString> &args = {})
      : AbstractAction(QCoreApplication::translate("EditShortcutAction", "Edit shortcut"),
                       ImageURL::builtin("pencil")),
        m_shortcut(shortcut) {}
};

struct RemoveShortcutAction : public AbstractAction {
  Q_DECLARE_TR_FUNCTIONS(RemoveShortcutAction)

  std::shared_ptr<Shortcut> m_shortcut;

public:
  void execute(ApplicationContext *ctx) override {
    auto shortcutDb = ctx->services->shortcuts();
    auto toast = ctx->services->toastService();
    bool removeResult = shortcutDb->removeShortcut(m_shortcut->id());

    if (removeResult) {
      toast->setToast(tr("Removed link"));
    } else {
      toast->setToast(tr("Failed to remove link"), ToastStyle::Danger);
    }
  }

  RemoveShortcutAction(const std::shared_ptr<Shortcut> &link)
      : AbstractAction(tr("Remove link"), ImageURL::builtin("trash")), m_shortcut(link) {
    setStyle(AbstractAction::Style::Danger);
    setShortcut(Keybind::DangerousRemoveAction);
  }
};

struct DuplicateShortcutAction : public AbstractAction {
public:
  std::shared_ptr<Shortcut> link;

  void execute(ApplicationContext *ctx) override {
    auto view = new ShortcutFormViewHost(link, ShortcutFormViewHost::Mode::Duplicate);

    ctx->navigation->pushView(view);
  }

  DuplicateShortcutAction(const std::shared_ptr<Shortcut> &link)
      : AbstractAction(QCoreApplication::translate("DuplicateShortcutAction", "Duplicate link"),
                       ImageURL::builtin("duplicate")),
        link(link) {}
};

/**
 * Submenu action to let the user select which app to open the shortcut
 * with. The list of available apps depends on the shortcut url.
 */
class OpenCompletedShortcutWithAction : public ListSubmenuAction {
public:
  OpenCompletedShortcutWithAction(const std::shared_ptr<Shortcut> &shortcut)
      : ListSubmenuAction(QCoreApplication::translate("OpenCompletedShortcutWithAction", "Open with..."),
                          BuiltinIcon::ArrowUp),
        m_shortcut(shortcut) {
    setShortcut(Keybind::OpenAction);
  }

  std::unique_ptr<ActionPanelState> buildState(ApplicationContext *ctx) const override {
    auto panel = std::make_unique<ActionPanelState>();
    auto section = panel->createSection();
    auto args = ctx->navigation->completionValues() |
                std::views::transform([](auto &&p) { return p.second; }) | std::ranges::to<std::vector>();
    auto expanded = expandShortcut(*m_shortcut, args);

    for (const auto &opener : ctx->services->appDb()->findOpeners(m_shortcut->url())) {
      section->addAction(new OpenAppAction(opener, opener->displayName(), {expanded}));
    }

    return panel;
  }

private:
  std::shared_ptr<Shortcut> m_shortcut;
};

class CopyShortcutAction : public AbstractAction {
  Q_DECLARE_TR_FUNCTIONS(CopyShortcutAction)

public:
  void execute(ApplicationContext *ctx) override {
    auto args = ctx->navigation->completionValues() |
                std::views::transform([](auto &&p) { return p.second; }) | std::ranges::to<std::vector>();
    auto expanded = expandShortcut(*m_shortcut, args);

    ctx->services->clipman()->copyText(expanded);
    ctx->navigation->showHud(tr("Copied to clipboard"));
  }

  CopyShortcutAction(const std::shared_ptr<Shortcut> &shortcut)
      : AbstractAction(tr("Copy shortcut"), BuiltinIcon::CopyClipboard), m_shortcut(shortcut) {}

private:
  std::shared_ptr<Shortcut> m_shortcut;
};
