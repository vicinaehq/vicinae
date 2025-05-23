#pragma once
#include "base-view.hpp"
#include "omni-icon.hpp"
#include "root-item-manager.hpp"
#include "service-registry.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/omni-list.hpp"
#include <qobjectdefs.h>
#include <ranges>

class FallbackListItem : public AbstractDefaultListItem, public ListView::Actionnable {
  std::shared_ptr<RootItem> m_item;

  ItemData data() const override {
    return {
        .iconUrl = m_item->iconUrl(),
        .name = m_item->displayName(),
        .category = m_item->subtitle(),
        .accessories = m_item->accessories(),
    };
  }

  QString id() const override { return m_item->uniqueId(); }

public:
  const RootItem &item() const { return *m_item.get(); }
  FallbackListItem(const std::shared_ptr<RootItem> &item) : m_item(item) {}
  ~FallbackListItem() {}
};

class ManageFallbackCommands : public ListView {
  void enableFallback(const QString &id) {
    auto itemManager = ServiceRegistry::instance()->rootItemManager();

    itemManager->setFallback(id);
    renderList(searchText(), OmniList::PreserveSelection);
  }

  void disableFallback(const QString &id) {
    auto itemManager = ServiceRegistry::instance()->rootItemManager();

    itemManager->disableFallback(id);
    renderList(searchText(), OmniList::PreserveSelection);
  }

  void moveFallbackUp(const QString &id) {
    auto itemManager = ServiceRegistry::instance()->rootItemManager();
    auto metadata = itemManager->itemMetadata(id);

    qDebug() << "old metadata" << metadata.fallbackPosition;

    itemManager->setFallback(id, std::max(0, metadata.fallbackPosition - 1));
    onSearchChanged(searchText());
  }

  void moveFallbackDown(const QString &id) {
    auto itemManager = ServiceRegistry::instance()->rootItemManager();
    auto metadata = itemManager->itemMetadata(id);

    itemManager->setFallback(id, metadata.fallbackPosition + 1);
    onSearchChanged(searchText());
  }

  QList<AbstractAction *> generateActiveFallbackActions(const QString &id) {
    auto disable = new StaticAction("Disable fallback", BuiltinOmniIconUrl("arrow-counter-clockwise"),
                                    [this, id]() { disableFallback(id); });
    auto moveUp = new StaticAction("Move fallback up", BuiltinOmniIconUrl("arrow-up"),
                                   [this, id]() { moveFallbackUp(id); });
    auto moveDown = new StaticAction("Move fallback down", BuiltinOmniIconUrl("arrow-down"),
                                     [this, id]() { moveFallbackDown(id); });

    return {disable, moveUp, moveDown};
  }

  QList<AbstractAction *> generateAvailableFallbackActions(const QString &id) {
    auto enable = new StaticAction("Enable fallback", BuiltinOmniIconUrl("arrow-counter-clockwise"),
                                   [this, id]() { enableFallback(id); });

    return {enable};
  }

  void onItemSelected(const OmniList::AbstractVirtualItem &item) override {
    auto itemManager = ServiceRegistry::instance()->rootItemManager();
    auto fallback = static_cast<const FallbackListItem &>(item);
    auto &rootItem = fallback.item();
    QList<AbstractAction *> actions;

    if (itemManager->isFallback(rootItem.uniqueId())) {
      actions = generateActiveFallbackActions(rootItem.uniqueId());
    } else {
      actions = generateAvailableFallbackActions(rootItem.uniqueId());
    }

    setActions(actions);
  }

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
      enabledSection.addItem(std::make_unique<FallbackListItem>(item));
    }

    auto available = fallbacks | std::views::filter([itemManager](const auto &item) {
                       return !itemManager->itemMetadata(item->uniqueId()).isFallback;
                     });

    auto &availableSection = m_list->addSection("Available");

    for (const auto &item : available) {
      availableSection.addItem(std::make_unique<FallbackListItem>(item));
    }
    m_list->endResetModel(selectionPolicy);

    if (auto item = m_list->selected()) { onItemSelected(*item); }
  }

  void onSearchChanged(const QString &text) override { renderList(text); }

  void initialize() override { onSearchChanged(""); }

public:
  ManageFallbackCommands() {
    setSearchPlaceholderText("Manage fallback commands...");
    setNavigationTitle("Manage Fallback Commands");
    setNavigationIcon(BuiltinOmniIconUrl("arrow-counter-clockwise").setBackgroundTint(ColorTint::Red));
  }
};
