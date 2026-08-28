#include "root-search/kde-settings/kde-settings-root-provider.hpp"
#include "actions/app/app-actions.hpp"
#include "actions/root-search/root-search-actions.hpp"
#include "clipboard-actions.hpp"
#include "service-registry.hpp"
#include "ui/action-pannel/action-panel-state.hpp"
#include "ui/image/url.hpp"

static const QString KCM_ID_PREFIX = QStringLiteral("kcm_");

QString KdeSettingsRootItem::title() const { return m_app->displayName(); }

QString KdeSettingsRootItem::typeDisplayName() const { return tr("KDE Settings"); }

ImageURL KdeSettingsRootItem::iconUrl() const { return m_app->iconUrl(); }

EntrypointId KdeSettingsRootItem::uniqueId() const {
  return EntrypointId("kde-settings", m_app->id().remove(".desktop").toStdString());
}

AccessoryList KdeSettingsRootItem::accessories() const {
  return {{.text = tr("KDE Settings"), .color = SemanticColor::TextMuted}};
}

std::vector<QString> KdeSettingsRootItem::keywords() const {
  auto keywords = m_app->keywords();

  if (auto name = m_app->unlocalizedName()) { keywords.emplace_back(name.value()); }

  return keywords;
}

std::vector<std::pair<QString, QString>> KdeSettingsRootItem::settingsMetadata() const {
  return {{QStringLiteral("ID"), m_app->id()},
          {tr("Name"), m_app->displayName()},
          {tr("Where"), QString::fromStdString(m_app->path().string())}};
}

std::unique_ptr<ActionPanelState>
KdeSettingsRootItem::newActionPanel(ApplicationContext *ctx, const RootItemMetadata &metadata) const {
  auto panel = std::make_unique<ListActionPanelState>();
  auto mainSection = panel->createSection();
  auto utils = panel->createSection();
  auto itemSection = panel->createSection();

  auto open = new OpenAppAction(m_app, tr("Open in System Settings"), {});
  open->setClearSearch(true);
  mainSection->addAction(open);

  utils->addAction(new CopyToClipboardAction(Clipboard::Text(m_app->id()), tr("Copy Module ID")));

  for (const auto &action :
       RootSearchActionGenerator::generateActions(*this, *ctx->services->rootItemManager())) {
    itemSection->addAction(action);
  }

  panel->setTitle(m_app->displayName());
  return panel;
}

KdeSettingsRootProvider::KdeSettingsRootProvider(AppService &appService) : m_appService(appService) {
  connect(&m_appService, &AppService::appsChanged, this, &KdeSettingsRootProvider::itemsChanged);
}

QString KdeSettingsRootProvider::uniqueId() const { return "kde-settings"; }

QString KdeSettingsRootProvider::displayName() const { return tr("KDE Settings"); }

QString KdeSettingsRootProvider::description() const {
  return tr("Modules of the KDE System Settings application.");
}

ImageURL KdeSettingsRootProvider::icon() const {
  return ImageURL::system("preferences-system").withFallback(ImageURL::builtin(BuiltinIcon::Cog));
}

RootProvider::Type KdeSettingsRootProvider::type() const { return RootProvider::Type::GroupProvider; }

std::vector<std::shared_ptr<RootItem>> KdeSettingsRootProvider::loadItems() const {
  // kcm modules are just desktop files with NoDisplay=true
  const auto apps = m_appService.list();
  std::vector<std::shared_ptr<RootItem>> items;

  items.reserve(apps.size());

  for (const auto &app : apps) {
    if (app->id().startsWith(KCM_ID_PREFIX)) {
      items.emplace_back(std::make_shared<KdeSettingsRootItem>(app));
    }
  }

  return items;
}
