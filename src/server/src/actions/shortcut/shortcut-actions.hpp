#pragma once
#include <QGuiApplication>
#include "common.hpp"
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

class OpenShortcutAction : public AbstractAction {
  std::shared_ptr<Shortcut> m_shortcut;
  std::vector<QString> m_arguments;
  std::shared_ptr<AbstractApplication> m_app;
  bool m_clearSearch = false;

public:
  void execute(ApplicationContext *ctx) override {
    auto appDb = ctx->services->appDb();
    auto toast = ctx->services->toastService();
    auto shortcut = ctx->services->shortcuts();
    QString expanded;
    size_t argumentIndex = 0;

    for (const auto &part : m_shortcut->parts()) {
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
          if (argumentIndex < m_arguments.size()) { expanded += m_arguments.at(argumentIndex++); }
        }
      }
    }

    if (m_app) {
      appDb->launch(*m_app, {expanded});
    } else if (auto app = appDb->findById(m_shortcut->app())) {
      appDb->launch(*app, {expanded});
    } else {
      toast->setToast("No app with id " + m_shortcut->app(), ToastStyle::Danger);
      return;
    }

    shortcut->registerVisit(m_shortcut->id());
    ctx->navigation->closeWindow();
    if (m_clearSearch) ctx->navigation->clearSearchText();
  }

  QString title() const override { return "Open shortcut"; }

public:
  void setClearSearch(bool value) { m_clearSearch = value; }

  OpenShortcutAction(const std::shared_ptr<Shortcut> &shortcut, const std::vector<QString> &arguments,
                     const std::shared_ptr<AbstractApplication> &app = nullptr)
      : AbstractAction("Open shortcut", shortcut->icon()), m_shortcut(shortcut), m_arguments(arguments),
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
      : AbstractAction("Open shortcut", shortcut->icon()), m_shortcut(shortcut), m_app(app) {}
};

class OpenShortcutFromSearchText : public AbstractAction {
  std::shared_ptr<Shortcut> m_shortcut;

  void execute(ApplicationContext *ctx) override {
    OpenShortcutAction open(m_shortcut, {ctx->navigation->searchText()});

    open.execute(ctx);
  }

public:
  OpenShortcutFromSearchText(const std::shared_ptr<Shortcut> &shortcut)
      : AbstractAction("Open shortcut", shortcut->icon()), m_shortcut(shortcut) {}
};

struct EditShortcutAction : public AbstractAction {
public:
  std::shared_ptr<Shortcut> m_shortcut;

  void execute(ApplicationContext *ctx) override {
    auto view = new ShortcutFormViewHost(m_shortcut, ShortcutFormViewHost::Mode::Edit);

    ctx->navigation->pushView(view);
  }

  EditShortcutAction(const std::shared_ptr<Shortcut> &shortcut, const QList<QString> &args = {})
      : AbstractAction("Edit shortcut", ImageURL::builtin("pencil")), m_shortcut(shortcut) {}
};

struct RemoveShortcutAction : public AbstractAction {
  std::shared_ptr<Shortcut> m_shortcut;

public:
  void execute(ApplicationContext *ctx) override {
    auto shortcutDb = ctx->services->shortcuts();
    auto toast = ctx->services->toastService();
    bool removeResult = shortcutDb->removeShortcut(m_shortcut->id());

    if (removeResult) {
      toast->setToast("Removed link");
    } else {
      toast->setToast("Failed to remove link", ToastStyle::Danger);
    }
  }

  RemoveShortcutAction(const std::shared_ptr<Shortcut> &link)
      : AbstractAction("Remove link", ImageURL::builtin("trash")), m_shortcut(link) {
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
      : AbstractAction("Duplicate link", ImageURL::builtin("duplicate")), link(link) {}
};

/**
 * Submenu action to let the user select which app to open the shortcut
 * with. The list of available apps depends on the shortcut url.
 */
class OpenCompletedShortcutWithAction : public AbstractAction {
  class OpenAction : public AbstractAction {
    std::shared_ptr<Shortcut> m_shortcut;
    std::shared_ptr<AbstractApplication> m_app;

    void execute(ApplicationContext *ctx) override {
      OpenCompletedShortcutAction(m_shortcut, m_app).execute(ctx);
    }

    QString id() const override { return m_app->id(); }

  public:
    OpenAction(const std::shared_ptr<Shortcut> &shortcut, const std::shared_ptr<AbstractApplication> &app)
        : AbstractAction(app->displayName(), app->iconUrl()), m_shortcut(shortcut), m_app(app) {}
  };

  std::shared_ptr<Shortcut> m_shortcut;

  bool isSubmenu() const override { return true; }

public:
  OpenCompletedShortcutWithAction(const std::shared_ptr<Shortcut> &shortcut)
      : AbstractAction("Open with...", ImageURL::builtin("arrow-clockwise")), m_shortcut(shortcut) {}
};
