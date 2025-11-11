#pragma once
#include "common.hpp"
#include "navigation-controller.hpp"
#include "ui/views/list-view.hpp"
#include "ui/image/url.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include "service-registry.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/omni-list/omni-list.hpp"
#include <qobjectdefs.h>
#include <ranges>
#include "ui/views/list-view.hpp"

class DisableFallbackAction : public AbstractAction {
  QString m_id;

  void execute(ApplicationContext *ctx) override {
    auto manager = ctx->services->rootItemManager();

    manager->disableFallback(m_id);
  }

  QString title() const override { return "Disable fallback"; }
  std::optional<ImageURL> icon() const override { return ImageURL::builtin("trash"); }

public:
  DisableFallbackAction(const QString &id) : m_id(id) {}
};

class MoveFallbackUpAction : public AbstractAction {
  QString m_id;

  void execute(ApplicationContext *ctx) override {
    auto manager = ctx->services->rootItemManager();
    int pos = manager->itemMetadata(m_id).fallbackPosition;

    manager->moveFallbackUp(m_id);
  }

  QString title() const override { return "Move fallback up"; }
  std::optional<ImageURL> icon() const override { return ImageURL::builtin("arrow-up"); }

public:
  MoveFallbackUpAction(const QString &id) : m_id(id) {}
};

class MoveFallbackDownAction : public AbstractAction {
  QString m_id;

  void execute(ApplicationContext *ctx) override {
    auto manager = ctx->services->rootItemManager();
    int pos = manager->itemMetadata(m_id).fallbackPosition;

    manager->moveFallbackDown(m_id);
  }

  QString title() const override { return "Move fallback down"; }
  std::optional<ImageURL> icon() const override { return ImageURL::builtin("arrow-down"); }

public:
  MoveFallbackDownAction(const QString &id) : m_id(id) {}
};

class EnableFallbackAction : public AbstractAction {
  QString m_id;

  void execute(ApplicationContext *ctx) override {
    auto manager = ctx->services->rootItemManager();

    manager->enableFallback(m_id);
  }

  QString title() const override { return "Enable fallback"; }
  std::optional<ImageURL> icon() const override { return ImageURL::builtin("checkmark"); }

public:
  EnableFallbackAction(const QString &id) : m_id(id) {}
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

  std::unique_ptr<ActionPanelState> newActionPanel(ApplicationContext *ctx) const override {
    auto panel = std::make_unique<ListActionPanelState>();
    auto manager = ctx->services->rootItemManager();
    auto section = panel->createSection();

    if (manager->isFallback(m_item->uniqueId())) {
      auto disable = new DisableFallbackAction(m_item->uniqueId());
      section->addAction(disable);
    } else {
      auto enable = new EnableFallbackAction(m_item->uniqueId());
      section->addAction(enable);
    }

    return panel;
  }

public:
  const RootItem &item() const { return *m_item.get(); }
  FallbackListItem(const std::shared_ptr<RootItem> &item) : m_item(item) {}
  ~FallbackListItem() {}
};

// Overrides item actions when no filtering is applied
class RootFallbackListItem : public FallbackListItem {
  std::unique_ptr<ActionPanelState> newActionPanel(ApplicationContext *ctx) const override {
    auto panel = std::make_unique<ListActionPanelState>();
    auto manager = ctx->services->rootItemManager();
    auto section = panel->createSection();
    auto metadata = manager->itemMetadata(m_item->uniqueId());
    int maxFallbackPosition = manager->maxFallbackPosition();

    if (metadata.isFallback()) {
      auto disableFallback = new DisableFallbackAction(m_item->uniqueId());

      section->addAction(disableFallback);
      if (metadata.fallbackPosition > 0) { section->addAction(new MoveFallbackUpAction(m_item->uniqueId())); }
      if (metadata.fallbackPosition < maxFallbackPosition) {
        section->addAction(new MoveFallbackDownAction(m_item->uniqueId()));
      }
    } else {
      auto enableFallback = new EnableFallbackAction(m_item->uniqueId());
      section->addAction(enableFallback);
    }

    return panel;
  }

public:
  RootFallbackListItem(const std::shared_ptr<RootItem> &item) : FallbackListItem(item) {}
};

class ManageFallbackCommandsView : public ListView {
  void renderList(const QString &text, OmniList::SelectionPolicy selectionPolicy = OmniList::SelectFirst) {
    QString query = text.trimmed();
    auto itemManager = ServiceRegistry::instance()->rootItemManager();
    auto results = itemManager->search(query);
    auto fallbacks = results | std::views::filter(
                                   [](const auto &item) { return item.item.get()->isSuitableForFallback(); });
    std::vector<std::shared_ptr<RootItem>> enabled;

    for (const auto &item : fallbacks) {
      if (itemManager->isFallback(item.item.get()->uniqueId())) { enabled.emplace_back(item.item.get()); };
    }

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
                       return !itemManager->isFallback(item.item.get()->uniqueId());
                     });

    auto &availableSection = m_list->addSection("Available");

    for (const auto &item : available) {
      availableSection.addItem(std::make_unique<FallbackListItem>(item.item.get()));
    }
    m_list->endResetModel(selectionPolicy);
  }

  void textChanged(const QString &text) override { renderList(text); }

  void reloadInPlace() { renderList(searchText(), OmniList::PreserveSelection); }

  void initialize() override {
    auto manager = ServiceRegistry::instance()->rootItemManager();

    setSearchPlaceholderText("Manage fallback commands...");
    textChanged("");
    connect(manager, &RootItemManager::fallbackEnabled, this, [this]() { reloadInPlace(); });
    connect(manager, &RootItemManager::fallbackDisabled, this, [this]() { reloadInPlace(); });
    connect(manager, &RootItemManager::fallbackOrderChanged, this, [this]() { reloadInPlace(); });
  }

public:
  ManageFallbackCommandsView() {
    setSearchPlaceholderText("Manage fallback commands...");
    setNavigationTitle("Manage Fallback Commands");
    setNavigationIcon(ImageURL::builtin("arrow-counter-clockwise").setBackgroundTint(SemanticColor::Red));
  }
};
