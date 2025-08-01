#pragma once
#include "ui/views/list-view.hpp"
#include "omni-icon.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include "service-registry.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/omni-list/omni-list.hpp"
#include <qobjectdefs.h>
#include <ranges>
#include "ui/views/list-view.hpp"

class DisableFallbackAction : public AbstractAction {
  QString m_id;

  // legacy, to be removed
  void execute(AppWindow &app) override {}

  void execute() override {
    auto manager = ServiceRegistry::instance()->rootItemManager();

    manager->disableFallback(m_id);
  }

public:
  DisableFallbackAction(const QString &id)
      : AbstractAction("Disable fallback", BuiltinOmniIconUrl("trash")), m_id(id) {}
};

class MoveFallbackUpAction : public AbstractAction {
  QString m_id;

  // legacy, to be removed
  void execute(AppWindow &app) override {}

  void execute() override {
    auto manager = ServiceRegistry::instance()->rootItemManager();
    int pos = manager->itemMetadata(m_id).fallbackPosition;

    manager->setFallback(m_id, std::max(0, pos - 1));
  }

public:
  MoveFallbackUpAction(const QString &id)
      : AbstractAction("Move fallback up", BuiltinOmniIconUrl("arrow-up")), m_id(id) {}
};

class MoveFallbackDownAction : public AbstractAction {
  QString m_id;

  void execute(AppWindow &app) override {}

  void execute() override {
    auto manager = ServiceRegistry::instance()->rootItemManager();
    int pos = manager->itemMetadata(m_id).fallbackPosition;

    manager->setFallback(m_id, pos + 1);
  }

public:
  MoveFallbackDownAction(const QString &id)
      : AbstractAction("Move fallback down", BuiltinOmniIconUrl("arrow-down")), m_id(id) {}
};

class EnableFallbackAction : public AbstractAction {
  QString m_id;

  void execute(AppWindow &app) override {}

  void execute() override {
    auto manager = ServiceRegistry::instance()->rootItemManager();

    manager->setFallback(m_id);
  }

public:
  EnableFallbackAction(const QString &id)
      : AbstractAction("Enable fallback", BuiltinOmniIconUrl("checkmark")), m_id(id) {}
};

class FallbackListItem : public AbstractDefaultListItem, public ListView::Actionnable {
protected:
  std::shared_ptr<RootItem> m_item;

  ItemData data() const override {
    return {
        .iconUrl = m_item->iconUrl(),
        .name = m_item->displayName(),
        .subtitle = m_item->subtitle(),
        .accessories = m_item->accessories(),
    };
  }

  QString generateId() const override {
    auto manager = ServiceRegistry::instance()->rootItemManager();
    if (manager->isFallback(m_item->uniqueId())) { return QString("fallback.%1").arg(m_item->uniqueId()); }
    return QString("available.%1").arg(m_item->uniqueId());
  }

  QList<AbstractAction *> generateActions() const override {
    auto manager = ServiceRegistry::instance()->rootItemManager();

    if (manager->isFallback(m_item->uniqueId())) {
      auto disable = new DisableFallbackAction(m_item->uniqueId());

      disable->setPrimary(true);
      disable->setShortcut({.key = "return"});

      return {disable};
    }

    auto enable = new EnableFallbackAction(m_item->uniqueId());

    enable->setPrimary(true);
    enable->setShortcut({.key = "return"});

    return {enable};
  }

public:
  const RootItem &item() const { return *m_item.get(); }
  FallbackListItem(const std::shared_ptr<RootItem> &item) : m_item(item) {}
  ~FallbackListItem() {}
};

// Overrides item actions when no filtering is applied
class RootFallbackListItem : public FallbackListItem {
  /*
QList<AbstractAction *> generateActions() const override {
auto manager = ServiceRegistry::instance()->rootItemManager();
auto metadata = manager->itemMetadata(m_item->uniqueId());
QList<AbstractAction *> actions;
int maxFallbackPosition = manager->maxFallbackPosition();

if (metadata.isFallback) {
auto disableFallback = new DisableFallbackAction(m_item->uniqueId());

disableFallback->setPrimary(true);
disableFallback->setShortcut({.key = "return"});
actions << disableFallback;

if (metadata.fallbackPosition > 0) { actions << new MoveFallbackUpAction(m_item->uniqueId()); }
if (metadata.fallbackPosition < maxFallbackPosition) {
  actions << new MoveFallbackDownAction(m_item->uniqueId());
}
} else {
auto enableFallback = new EnableFallbackAction(m_item->uniqueId());

enableFallback->setPrimary(true);
enableFallback->setShortcut({.key = "return"});
actions << enableFallback;
}

return actions;
}
*/

public:
  RootFallbackListItem(const std::shared_ptr<RootItem> &item) : FallbackListItem(item) {}
};

class ManageFallbackCommandsView : public ListView {
  void renderList(const QString &text, OmniList::SelectionPolicy selectionPolicy = OmniList::SelectFirst) {
    qDebug() << "render list";
    QString query = text.trimmed();
    auto itemManager = ServiceRegistry::instance()->rootItemManager();
    auto results = query.isEmpty() ? itemManager->allItems() : itemManager->prefixSearch(query);
    auto fallbacks =
        results | std::views::filter([](const auto &item) { return item->isSuitableForFallback(); });

    auto enabled = fallbacks | std::views::filter([itemManager](const auto &item) {
                     return itemManager->isFallback(item->uniqueId());
                   }) |
                   std::ranges::to<std::vector>();
    std::ranges::sort(enabled, [itemManager](const auto &a, const auto &b) {
      auto ma = itemManager->itemMetadata(a->uniqueId());
      auto mb = itemManager->itemMetadata(b->uniqueId());

      return ma.fallbackPosition < mb.fallbackPosition;
    });

    m_list->beginResetModel();
    auto &enabledSection = m_list->addSection("Enabled");

    for (const auto &item : enabled) {
      if (query.isEmpty()) {
        enabledSection.addItem(std::make_unique<RootFallbackListItem>(item));
      } else {
        enabledSection.addItem(std::make_unique<FallbackListItem>(item));
      }
    }

    auto available = fallbacks | std::views::filter([itemManager](const auto &item) {
                       return !itemManager->itemMetadata(item->uniqueId()).isFallback;
                     });

    auto &availableSection = m_list->addSection("Available");

    for (const auto &item : available) {
      availableSection.addItem(std::make_unique<FallbackListItem>(item));
    }
    m_list->endResetModel(selectionPolicy);
  }

  void textChanged(const QString &text) override { renderList(text); }

  void initialize() override {
    auto manager = ServiceRegistry::instance()->rootItemManager();

    textChanged("");
    connect(manager, &RootItemManager::fallbackEnabled, this,
            [this]() { renderList(searchText(), OmniList::PreserveSelection); });
    connect(manager, &RootItemManager::fallbackDisabled, this,
            [this]() { renderList(searchText(), OmniList::PreserveSelection); });
  }

public:
  ManageFallbackCommandsView() {
    setSearchPlaceholderText("Manage fallback commands...");
    setNavigationTitle("Manage Fallback Commands");
    setNavigationIcon(BuiltinOmniIconUrl("arrow-counter-clockwise").setBackgroundTint(SemanticColor::Red));
  }
};
