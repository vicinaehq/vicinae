#pragma once
#include "actions/root-search/root-search-actions.hpp"
#include "app/app-database.hpp"
#include "root-item-manager.hpp"
#include "service-registry.hpp"
#include <ranges>

class OpenAppAction : public AbstractAction {
  std::shared_ptr<Application> application;
  std::vector<QString> args;

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

    ActionPanelView *actionPanel() const override {
      auto panel = new ActionPanelStaticListView;
      auto appDb = ServiceRegistry::instance()->appDb();
      auto fileBrowser = appDb->appProvider()->fileBrowser();
      auto textEditor = appDb->appProvider()->textEditor();
      auto open = new OpenAppAction(m_app, "Open Application", {});
      auto actions = m_app->actions();

      panel->setTitle(m_app->name());
      panel->addAction(new DefaultActionWrapper(uniqueId(), open));

      auto makeAction = [](auto &&pair) -> OpenAppAction * {
        const auto &[index, appAction] = pair;
        auto openAction = new OpenAppAction(appAction, appAction->name(), {});

        if (index < 9) {
          openAction->setShortcut({.key = QString::number(index + 1), .modifiers = {"ctrl", "shift"}});
        }

        return openAction;
      };

      std::ranges::for_each(m_app->actions() | std::views::enumerate | std::views::transform(makeAction),
                            [&](auto &&action) { panel->addAction(action); });

      if (fileBrowser) {
        auto openLocation = new OpenAppAction(fileBrowser, "Open Location", {m_app->path().c_str()});

        openLocation->setShortcut({.key = "O", .modifiers = {"ctrl"}});
        panel->addAction(openLocation);
      }

      auto resetRanking = new ResetItemRanking(uniqueId());
      auto markAsFavorite = new MarkItemAsFavorite(uniqueId());

      panel->addSection();
      panel->addAction(resetRanking);
      panel->addAction(markAsFavorite);
      panel->addSection();

      auto disable = new DisableApplication(uniqueId());

      panel->addAction(disable);

      return panel;
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
