#include "root-search/apps/app-root-provider.hpp"
#include "actions/app/app-actions.hpp"
#include "actions/root-search/root-search-actions.hpp"
#include "clipboard-actions.hpp"
#include "common.hpp"
#include "navigation-controller.hpp"
#include "ui/image/url.hpp"
#include "service-registry.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include "services/window-manager/window-manager.hpp"
#include "services/app-runtime/app-runtime.hpp"
#include "vicinae.hpp"
#include "actions/wm/window-actions.hpp"
#include "utils/environment.hpp"
#include <qjsonobject.h>
#include <qkeysequence.h>

double AppRootItem::baseScoreWeight() const { return 1; }

QString AppRootItem::typeDisplayName() const {
  const QString category = m_app->category();
  return category.isEmpty() ? tr("Application") : category;
}

std::vector<QString> AppRootItem::keywords() const {
  auto keywords = m_app->keywords();
  keywords.emplace_back(m_app->description());

  if (auto name = m_app->unlocalizedName()) { keywords.emplace_back(name.value()); }

  return keywords;
}

QString AppRootItem::subtitle() const { return QString(); }

QString AppRootItem::title() const { return m_app->displayName(); }

QString AppRootItem::settingsDescription() const { return m_app->description(); }

std::vector<std::pair<QString, QString>> AppRootItem::settingsMetadata() const {
  return {{tr("ID"), m_app->id()},
          {tr("Name"), m_app->displayName()},
          {tr("Where"), QString::fromStdString(m_app->path().string())},
          {tr("Opens in terminal"), m_app->isTerminalApp() ? tr("Yes") : tr("No")}};
}

bool AppRootItem::isActive() const { return ServiceRegistry::instance()->appRuntime()->isRunning(*m_app); }

AccessoryList AppRootItem::accessories() const {
  const QString category = m_app->category();
  return {{.text = category.isEmpty() ? tr("Application") : category, .color = SemanticColor::TextMuted}};
}

EntrypointId AppRootItem::uniqueId() const {
  return EntrypointId("applications", m_app->id().remove(".desktop").toStdString());
}

ImageURL AppRootItem::iconUrl() const { return m_app->iconUrl(); }

std::unique_ptr<ActionPanelState> AppRootItem::newActionPanel(ApplicationContext *ctx,
                                                              const RootItemMetadata &metadata) const {
  auto panel = std::make_unique<ListActionPanelState>();
  auto appDb = ctx->services->appDb();
  auto open = new OpenAppAction(m_app, tr("Open Application"), {});
  auto copyId = new CopyToClipboardAction(Clipboard::Text(m_app->id()), tr("Copy App ID"));
  auto copyLocation = new CopyToClipboardAction(
      Clipboard::Text(QString::fromStdString(m_app->path().string())), tr("Copy App Location"));
  auto preferences = ctx->services->rootItemManager()->getPreferenceValues(uniqueId());
  QString const defaultAction = preferences.value("defaultAction").toString();

  auto mainSection = panel->createSection();
  auto utils = panel->createSection();
  auto lifecycleSection = panel->createSection();
  auto itemSection = panel->createSection();
  auto appActions = m_app->actions();

  open->setClearSearch(true);
  panel->setTitle(m_app->displayName());

  auto activeWindows = ctx->services->windowManager()->findAppWindows(*m_app);

  if (!activeWindows.empty()) {
    auto focus = new FocusWindowAction(activeWindows.front());
    if (defaultAction == "focus") {
      mainSection->addAction(focus);
      mainSection->addAction(open);
    } else {
      mainSection->addAction(open);
      mainSection->addAction(focus);
    }

    auto provider = ctx->services->windowManager()->provider();
    if (provider->supportsSetSticky()) { mainSection->addAction(new PinWindowAction(activeWindows.front())); }
    if (provider->supportsMoveToWorkspace()) {
      mainSection->addAction(new BringToWorkspaceAction(activeWindows.front()));
    }
    mainSection->addAction(new CloseWindowAction(activeWindows.front()));
  } else {
    mainSection->addAction(open);
  }

  auto actions = m_app->actions();

  for (size_t i = 0; i != appActions.size(); ++i) {
    const auto &action = actions[i];
    auto openAction = new OpenAppAction(action, action->displayName(), {});

    if (i < 9) { openAction->setShortcut(QString("ctrl+alt+%1").arg(i + 1)); }
    mainSection->addAction(openAction);
  }

  if (auto opener = appDb->provider()->locationOpener(*m_app)) {
    auto openLocation = new OpenAppLocationAction(m_app, opener);
    openLocation->setShortcut(Keybind::OpenAction);
    utils->addAction(openLocation);
  }

  utils->addAction(copyId);
  utils->addAction(copyLocation);

  if (ctx->services->appRuntime()->isRunning(*m_app)) {
    auto quit = new QuitAppAction(m_app);
    quit->setShortcut(QString("ctrl+q"));
    lifecycleSection->addAction(quit);
    lifecycleSection->addAction(new ForceQuitAppAction(m_app));
  }

  for (const auto &action : RootSearchActionGenerator::generateActions(*this, metadata)) {
    itemSection->addAction(action);
  }

  return panel;
}

RootProvider::Type AppRootProvider::type() const { return RootProvider::Type::GroupProvider; }

ImageURL AppRootProvider::icon() const {
  return ImageURL::builtin("app-window-grid-2x2").setBackgroundTint(Omnicast::ACCENT_COLOR);
}

QString AppRootProvider::displayName() const {
  return QCoreApplication::translate("AppRootProvider", "Applications");
}

QString AppRootProvider::uniqueId() const { return "applications"; }

std::vector<std::shared_ptr<RootItem>> AppRootProvider::loadItems() const {
  std::vector<std::shared_ptr<RootItem>> items;

  for (const auto &app : m_appService.list()) {
    if (app->displayable()) { items.emplace_back(std::make_shared<AppRootItem>(app)); }
  }

  return items;
}

AppRootProvider::AppRootProvider(AppService &appService) : m_appService(appService) {
  connect(&m_appService, &AppService::appsChanged, this, &AppRootProvider::itemsChanged);
  if (auto runtime = ServiceRegistry::instance()->appRuntime()) {
    connect(runtime, &AppRuntime::runningAppsChanged, this, &AppRootProvider::itemsChanged);
  }
}

PreferenceList AppRootProvider::preferences() const { return m_appService.provider()->preferences(); }

void AppRootProvider::preferencesChanged(const QJsonObject &preferences) {
  m_appService.provider()->applyPreferences(preferences);
}
