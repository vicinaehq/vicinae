#include "root-search/apps/app-root-provider.hpp"
#include "actions/app/app-actions.hpp"
#include "actions/root-search/root-search-actions.hpp"
#include "clipboard-actions.hpp"
#include "navigation-controller.hpp"
#include "ui/image/url.hpp"
#include "service-registry.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include "settings/app-metadata-settings-detail.hpp"
#include "settings/app-settings-detail.hpp"
#include "services/window-manager/window-manager.hpp"
#include "switch-windows-view.hpp"
#include <qwidget.h>

namespace fs = std::filesystem;

double AppRootItem::baseScoreWeight() const { return 1; }

QString AppRootItem::typeDisplayName() const { return "Application"; }

std::vector<QString> AppRootItem::keywords() const { return m_app->keywords(); }

QString AppRootItem::subtitle() const { return m_app->description(); }

QString AppRootItem::providerId() const { return "app"; }

QString AppRootItem::displayName() const { return m_app->displayName(); }

QWidget *AppRootItem::settingsDetail(const QJsonObject &preferences) const {
  return new AppMetadataSettingsDetail(m_app);
}

bool AppRootItem::isActive() const {
  auto wm = ServiceRegistry::instance()->windowManager();
  return !wm->findAppWindows(*m_app).empty();
}

AccessoryList AppRootItem::accessories() const {
  return {{.text = "Application", .color = SemanticColor::TextSecondary}};
}

QString AppRootItem::uniqueId() const { return QString("apps.%1").arg(m_app->id()); }

ImageURL AppRootItem::iconUrl() const { return m_app->iconUrl(); }

std::unique_ptr<ActionPanelState> AppRootItem::newActionPanel(ApplicationContext *ctx,
                                                              const RootItemMetadata &metadata) {
  auto panel = std::make_unique<ActionPanelState>();
  auto appDb = ctx->services->appDb();
  auto open = new OpenAppAction(m_app, "Open Application", {});
  auto copyId = new CopyToClipboardAction(Clipboard::Text(m_app->id()), "Copy App ID");
  auto copyLocation = new CopyToClipboardAction(Clipboard::Text(m_app->path().c_str()), "Copy App Location");
  auto resetRanking = new ResetItemRanking(uniqueId());
  auto markAsFavorite = new ToggleItemAsFavorite(uniqueId(), metadata.favorite);
  auto disable = new DisableApplication(uniqueId());

  auto mainSection = panel->createSection();
  auto utils = panel->createSection();
  auto itemSection = panel->createSection();
  auto dangerSection = panel->createSection();
  auto appActions = m_app->actions();

  open->setClearSearch(true);
  panel->setTitle(m_app->displayName());

  auto activeWindows = ctx->services->windowManager()->findAppWindows(*m_app);

  if (!activeWindows.empty()) {
    auto focus = new FocusWindowAction(activeWindows.front());
    mainSection->addAction(new DefaultActionWrapper(uniqueId(), focus));
    mainSection->addAction(open);
  } else {
    mainSection->addAction(new DefaultActionWrapper(uniqueId(), open));
  }

  auto makeAction = [](auto &&pair) -> OpenAppAction * {
    const auto &[index, appAction] = pair;
    auto openAction = new OpenAppAction(appAction, appAction->name(), {});

    if (index < 9) {
      openAction->setShortcut({.key = QString::number(index + 1), .modifiers = {"ctrl", "shift"}});
    }

    return openAction;
  };

  auto actions = m_app->actions();

  for (int i = 0; i != appActions.size(); ++i) {
    auto action = actions[i];
    auto openAction = new OpenAppAction(action, action->displayName(), {});

    if (i < 9) { openAction->setShortcut(QString("ctrl+shift+%1").arg(i + 1)); }
    mainSection->addAction(openAction);
  }

  if (auto opener = appDb->findDefaultOpener(m_app->path().c_str())) {
    auto openLocation = new OpenAppAction(opener, "Open Location", {m_app->path().c_str()});
    openLocation->setShortcut(Keyboard::Shortcut::open());
    utils->addAction(openLocation);
  }

  utils->addAction(copyId);
  utils->addAction(copyLocation);
  itemSection->addAction(resetRanking);
  itemSection->addAction(markAsFavorite);
  dangerSection->addAction(disable);

  return panel;
}

RootProvider::Type AppRootProvider::type() const { return RootProvider::Type::GroupProvider; }

ImageURL AppRootProvider::icon() const { return ImageURL::builtin("folder"); }

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
  std::vector<std::shared_ptr<RootItem>> items;

  for (const auto &app : m_appService.list()) {
    if (app->displayable()) { items.emplace_back(std::make_shared<AppRootItem>(app)); }
  }

  return items;
}

AppRootProvider::AppRootProvider(AppService &appService) : m_appService(appService) {
  connect(&m_appService, &AppService::appsChanged, this, &AppRootProvider::itemsChanged);
}

void AppRootProvider::preferencesChanged(const QJsonObject &preferences) {
  QJsonArray jsonPaths = preferences.value("paths").toArray();

  if (!jsonPaths.empty()) {
    std::vector<fs::path> paths;
    paths.reserve(jsonPaths.size());

    for (const auto &jsonPath : jsonPaths) {
      paths.emplace_back(jsonPath.toString().toStdString());
    }

    m_appService.setAdditionalSearchPaths(paths);
    m_appService.scanSync();
  }
}
