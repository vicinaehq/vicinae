#pragma once
#include "app.hpp"
#include "app/app-database.hpp"
#include "root-item-manager.hpp"
#include "service-registry.hpp"

class OpenAppAction : public AbstractAction {
  std::shared_ptr<Application> application;
  std::vector<QString> args;

  void execute(AppWindow &app) override {}

  void execute() override {
    qDebug() << "execute app";
    auto appDb = ServiceRegistry::instance()->appDb();
    auto ui = ServiceRegistry::instance()->UI();

    if (!appDb->launch(*application.get(), args)) {
      qDebug() << "Failed to launch app";
      ui->setToast("Failed to start app", ToastPriority::Danger);
      return;
    }

    ui->popToRoot();
    ui->closeWindow();
  }

public:
  OpenAppAction(const std::shared_ptr<Application> &app, const QString &title,
                const std::vector<QString> args)
      : AbstractAction(title, app->iconUrl()), application(app), args(args) {
    qDebug() << "open app for" << app->name();
  }
  virtual ~OpenAppAction() {}
};

class AppRootProvider : public RootProvider {
public:
  class AppRootItem : public RootItem {
    std::shared_ptr<Application> m_app;

    double baseScoreWeight() const override { return 1; }

    QString providerId() const override { return "app"; }

    QString displayName() const override { return m_app->name(); }
    QList<AbstractAction *> actions() const override {
      auto appDb = ServiceRegistry::instance()->appDb();

      QList<AbstractAction *> actions;
      auto fileBrowser = appDb->appProvider()->fileBrowser();
      auto textEditor = appDb->appProvider()->textEditor();

      actions << new OpenAppAction(m_app, "Open Application", {});

      for (const auto &desktopAction : m_app->actions()) {
        actions << new OpenAppAction(desktopAction, desktopAction->name(), {});
      }

      if (fileBrowser) { actions << new OpenAppAction(fileBrowser, "Open in folder", {m_app->id()}); }
      if (textEditor) { actions << new OpenAppAction(textEditor, "Open desktop file", {m_app->id()}); }

      return actions;
    }
    AccessoryList accessories() const override {
      return {{.text = "Application", .color = ColorTint::TextSecondary}};
    }
    QString uniqueId() const override { return QString("apps.%1").arg(m_app->id()); }
    OmniIconUrl iconUrl() const override { return m_app->iconUrl(); }
    std::vector<QString> keywords() const override { return m_app->keywords(); }

  public:
    const Application &app() const { return *m_app.get(); }
    AppRootItem(const std::shared_ptr<Application> &app) : m_app(app) {}
  };

  AppService &m_appService;

  std::vector<std::shared_ptr<RootItem>> loadItems() const override {
    return m_appService.list() | std::views::filter([](const auto &app) { return app->displayable(); }) |
           std::views::transform([](const auto &app) {
             return std::static_pointer_cast<RootItem>(std::make_shared<AppRootItem>(app));
           }) |
           std::ranges::to<std::vector>();
  }

  Type type() const override { return RootProvider::Type::GroupProvider; }

  QString displayName() const override { return "Applications"; }

  QString uniqueId() const override { return "apps"; }

public:
  AppRootProvider(AppService &appService) : m_appService(appService) {}
};
