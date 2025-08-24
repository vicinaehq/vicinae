#include "root-search/shortcuts/shortcut-root-provider.hpp"
#include "action-panel/action-panel.hpp"
#include "actions/shortcut/shortcut-actions.hpp"
#include "actions/fallback-actions.hpp"
#include "actions/root-search/root-search-actions.hpp"
#include "argument.hpp"
#include "../../ui/image/url.hpp"
#include "services/shortcut/shortcut-service.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include "vicinae.hpp"
#include <memory>
#include <qlogging.h>
#include <ranges>

std::unique_ptr<ActionPanelState> RootShortcutItem::newActionPanel(ApplicationContext *ctx,
                                                                   const RootItemMetadata &metadata) {
  auto panel = std::make_unique<ActionPanelState>();
  auto mainSection = panel->createSection();
  auto itemSection = panel->createSection();
  auto dangerSection = panel->createSection();

  auto open = new OpenCompletedShortcutAction(m_link);
  auto edit = new EditShortcutAction(m_link);
  auto duplicate = new DuplicateShortcutAction(m_link);
  auto remove = new RemoveShortcutAction(m_link);

  auto resetRanking = new ResetItemRanking(uniqueId());
  auto markAsFavorite = new ToggleItemAsFavorite(uniqueId(), metadata.favorite);

  auto disable = new DisableItemAction(uniqueId());

  open->setClearSearch(true);
  open->setPrimary(true);
  open->setShortcut({.key = "return"});
  // openWith->setShortcut({.key = "return", .modifiers = {"shift"}});
  duplicate->setShortcut({.key = "N", .modifiers = {"ctrl"}});
  edit->setShortcut({.key = "E", .modifiers = {"ctrl"}});
  remove->setShortcut({.key = "X", .modifiers = {"ctrl"}});
  disable->setShortcut({.key = "X", .modifiers = {"ctrl", "shift"}});

  panel->setTitle(m_link->name());
  mainSection->addAction(new DefaultActionWrapper(uniqueId(), open));
  // mainSection->addAction(openWith);
  mainSection->addAction(edit);
  mainSection->addAction(duplicate);

  itemSection->addAction(resetRanking);
  itemSection->addAction(markAsFavorite);

  dangerSection->addAction(remove);
  dangerSection->addAction(disable);

  return panel;
}

ActionPanelView *RootShortcutItem::fallbackActionPanel() const {
  auto panel = new ActionPanelStaticListView;
  auto open = new OpenShortcutFromSearchText(m_link);
  auto manage = new ManageFallbackActions;

  open->setShortcut({.key = "return"});
  manage->setShortcut({.key = "return", .modifiers = {"shift"}});
  open->setPrimary(true);
  panel->setTitle(m_link->name());
  panel->addAction(open);
  panel->addAction(manage);

  return panel;
}

QString RootShortcutItem::typeDisplayName() const { return "Shortcut"; }

QString RootShortcutItem::uniqueId() const { return QString("shortcuts.%1").arg(m_link->id()); }

QString RootShortcutItem::displayName() const { return m_link->name(); }

double RootShortcutItem::baseScoreWeight() const { return 1.4; }

QString RootShortcutItem::providerId() const { return "shortcut"; }

AccessoryList RootShortcutItem::accessories() const {
  return {{.text = "Shortcut", .color = SemanticColor::TextSecondary}};
}

bool RootShortcutItem::isSuitableForFallback() const { return m_link->arguments().size() == 1; }

ArgumentList RootShortcutItem::arguments() const {
  auto mapArg = [](const Shortcut::Argument &arg) -> CommandArgument {
    CommandArgument cmdArg;

    cmdArg.type = CommandArgument::Text;
    cmdArg.required = arg.defaultValue.isEmpty();
    cmdArg.placeholder = arg.name;

    return cmdArg;
  };

  return m_link->arguments() | std::views::transform(mapArg) | std::ranges::to<std::vector>();
}

ImageURL RootShortcutItem::iconUrl() const {
  ImageURL url(m_link->icon());

  if (url.type() == ImageURLType::Builtin) { url.setBackgroundTint(Omnicast::ACCENT_COLOR); }

  return url;
}

RootShortcutItem::RootShortcutItem(const std::shared_ptr<Shortcut> &link) : m_link(link) {}

std::vector<std::shared_ptr<RootItem>> ShortcutRootProvider::loadItems() const {
  auto mapShortcut = [](auto &&shortcut) -> std::shared_ptr<RootItem> {
    return std::make_shared<RootShortcutItem>(shortcut);
  };

  return m_db.shortcuts() | std::views::transform(mapShortcut) | std::ranges::to<std::vector>();
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
