#include "root-search/apps/app-root-provider.hpp"
#include "actions/app/app-actions.hpp"
#include "actions/root-search/root-search-actions.hpp"
#include "omni-icon.hpp"
#include "root-item-manager.hpp"
#include "service-registry.hpp"
#include "settings/app-metadata-settings-detail.hpp"
#include "settings/app-settings-detail.hpp"
#include <qwidget.h>

double AppRootItem::baseScoreWeight() const { return 1; }

QString AppRootItem::typeDisplayName() const { return "Application"; }

std::vector<QString> AppRootItem::keywords() const { return m_app->keywords(); }

QString AppRootItem::providerId() const { return "app"; }

QString AppRootItem::displayName() const { return m_app->name(); }

QWidget *AppRootItem::settingsDetail(const QJsonObject &preferences) const {
  return new AppMetadataSettingsDetail(m_app);
}

AccessoryList AppRootItem::accessories() const {
  return {{.text = "Application", .color = ColorTint::TextSecondary}};
}

QString AppRootItem::uniqueId() const { return QString("apps.%1").arg(m_app->id()); }

OmniIconUrl AppRootItem::iconUrl() const { return m_app->iconUrl(); }

ActionPanelView *AppRootItem::actionPanel() const {
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

  panel->addSection();

  for (const auto &action : m_app->actions() | std::views::enumerate | std::views::transform(makeAction)) {
    panel->addAction(action);
  }

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

RootProvider::Type AppRootProvider::type() const { return RootProvider::Type::GroupProvider; }

OmniIconUrl AppRootProvider::icon() const { return BuiltinOmniIconUrl("folder"); }

QString AppRootProvider::displayName() const { return "Applications"; }

QJsonObject AppRootProvider::generateDefaultPreferences() const {
  QJsonObject preferences;
  QJsonArray paths;

  for (const auto &searchPath : m_appService.defaultSearchPaths()) {
    paths.push_back(QString::fromStdString(searchPath));
  }

  preferences["paths"] = paths;

  return preferences;
}

QWidget *AppRootProvider::settingsDetail() const { return new AppSettingsDetail; }

QString AppRootProvider::uniqueId() const { return "apps"; }

std::vector<std::shared_ptr<RootItem>> AppRootProvider::loadItems() const {
  auto isDisplayable = [](const auto &app) { return app->displayable(); };
  auto mapApp = [](const auto &app) -> std::shared_ptr<RootItem> {
    return std::make_shared<AppRootItem>(app);
  };

  return m_appService.list() | std::views::filter(isDisplayable) | std::views::transform(mapApp) |
         std::ranges::to<std::vector>();
}

AppRootProvider::AppRootProvider(AppService &appService) : m_appService(appService) {}
