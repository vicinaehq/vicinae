#pragma once
#include "base-view.hpp"
#include "root-item-manager.hpp"
#include "service-registry.hpp"
#include <qobjectdefs.h>
#include <ranges>

class FallbackListItem : public AbstractDefaultListItem, public ListView::Actionnable {
  std::shared_ptr<RootItem> m_item;

  QList<AbstractAction *> generateActions() const override {
    auto baseActions = m_item->actions();
    QList<AbstractAction *> finalActions;

    return finalActions;
  }

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
  FallbackListItem(const std::shared_ptr<RootItem> &item) : m_item(item) {}
  ~FallbackListItem() {}
};

class ManageFallbackCommands : public ListView {
  void onSearchChanged(const QString &text) override {
    QString query = text.trimmed();
    auto itemManager = ServiceRegistry::instance()->rootItemManager();
    auto results = itemManager->prefixSearch(query);

    auto fallbacks =
        results | std::views::filter([](const auto &item) { return item->isSuitableForFallback(); });
    auto enabled = fallbacks | std::views::filter([itemManager](const auto &item) {
                     return itemManager->itemMetadata(item->uniqueId()).isFallback;
                   });
    auto available = fallbacks | std::views::filter([itemManager](const auto &item) {
                       return !itemManager->itemMetadata(item->uniqueId()).isFallback;
                     });

    m_list->beginResetModel();
    auto &enabledSection = m_list->addSection("Enabled");

    for (const auto &item : enabled) {
      enabledSection.addItem(std::make_unique<FallbackListItem>(item));
    }

    auto &availableSection = m_list->addSection("Available");

    for (const auto &item : available) {
      availableSection.addItem(std::make_unique<FallbackListItem>(item));
    }
    m_list->endResetModel(OmniList::SelectFirst);
  }

  void initialize() override { onSearchChanged(""); }

public:
  ManageFallbackCommands() {}
};
