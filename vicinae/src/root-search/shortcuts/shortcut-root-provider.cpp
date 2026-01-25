#include "root-search/shortcuts/shortcut-root-provider.hpp"
#include "actions/shortcut/shortcut-actions.hpp"
#include "actions/fallback-actions.hpp"
#include "actions/root-search/root-search-actions.hpp"
#include "argument.hpp"
#include "common.hpp"
#include "navigation-controller.hpp"
#include "ui/image/url.hpp"
#include "services/shortcut/shortcut-service.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include "vicinae.hpp"
#include <memory>
#include <qlogging.h>

std::unique_ptr<ActionPanelState> RootShortcutItem::newActionPanel(ApplicationContext *ctx,
                                                                   const RootItemMetadata &metadata) const {
  auto panel = std::make_unique<ListActionPanelState>();
  auto mainSection = panel->createSection();
  auto itemSection = panel->createSection();
  auto dangerSection = panel->createSection();

  auto open = new OpenCompletedShortcutAction(m_link);
  auto edit = new EditShortcutAction(m_link);
  auto duplicate = new DuplicateShortcutAction(m_link);
  auto remove = new RemoveShortcutAction(m_link);

  open->setClearSearch(true);
  duplicate->setShortcut(Keybind::DuplicateAction);
  edit->setShortcut(Keybind::EditAction);
  remove->setShortcut(Keybind::DangerousRemoveAction);

  panel->setTitle(m_link->name());
  mainSection->addAction(new DefaultActionWrapper(uniqueId(), open));
  mainSection->addAction(edit);
  mainSection->addAction(duplicate);

  for (const auto action : RootSearchActionGenerator::generateActions(*this, metadata)) {
    itemSection->addAction(action);
  }

  dangerSection->addAction(remove);

  return panel;
}

std::unique_ptr<ActionPanelState>
RootShortcutItem::fallbackActionPanel(ApplicationContext *ctx, const RootItemMetadata &metadata) const {
  auto panel = std::make_unique<ActionPanelState>();
  auto main = panel->createSection();

  auto open = new OpenShortcutFromSearchText(m_link);
  auto manage = new ManageFallbackActions;

  panel->setTitle(m_link->name());
  main->addAction(open);
  main->addAction(manage);

  return panel;
}

QString RootShortcutItem::typeDisplayName() const { return "Shortcut"; }

EntrypointId RootShortcutItem::uniqueId() const {
  return EntrypointId{"shortcuts", m_link->id().toStdString()};
}

QString RootShortcutItem::displayName() const { return m_link->name(); }

double RootShortcutItem::baseScoreWeight() const { return 1.4; }

AccessoryList RootShortcutItem::accessories() const {
  return {{.text = "Shortcut", .color = SemanticColor::TextMuted}};
}

bool RootShortcutItem::isSuitableForFallback() const { return m_link->arguments().size() == 1; }

ArgumentList RootShortcutItem::arguments() const {
  ArgumentList args;

  for (const auto &arg : m_link->arguments()) {
    CommandArgument cmdArg;

    cmdArg.type = CommandArgument::Text;
    cmdArg.required = arg.defaultValue.isEmpty();
    cmdArg.placeholder = arg.name;
    args.emplace_back(cmdArg);
  }

  return args;
}

ImageURL RootShortcutItem::iconUrl() const {
  ImageURL url(m_link->icon());

  if (url.type() == ImageURLType::Builtin) { url.setBackgroundTint(Omnicast::ACCENT_COLOR); }

  return url;
}

RootShortcutItem::RootShortcutItem(const std::shared_ptr<Shortcut> &link) : m_link(link) {}

std::vector<std::shared_ptr<RootItem>> ShortcutRootProvider::loadItems() const {
  std::vector<std::shared_ptr<RootItem>> items;

  for (const auto &shortcut : m_db.shortcuts()) {
    items.emplace_back(std::make_shared<RootShortcutItem>(shortcut));
  }

  return items;
};

QString ShortcutRootProvider::displayName() const { return "Shortcuts"; }

ImageURL ShortcutRootProvider::icon() const {
  auto icon = ImageURL::builtin("link");

  icon.setBackgroundTint(Omnicast::ACCENT_COLOR);

  return icon;
}

QString ShortcutRootProvider::uniqueId() const { return "shortcuts"; }
RootProvider::Type ShortcutRootProvider::type() const { return RootProvider::Type::GroupProvider; }

ShortcutRootProvider::ShortcutRootProvider(ShortcutService &db) : m_db(db) {
  connect(&db, &ShortcutService::shortcutSaved, this, [this]() { emit itemsChanged(); });
  connect(&db, &ShortcutService::shortcutRemoved, this, [this]() { emit itemsChanged(); });
  connect(&db, &ShortcutService::shortcutUpdated, this, [this]() { emit itemsChanged(); });
}
