#pragma once
#include <algorithm>
#include <cstdint>
#include <memory>
#include "common.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/views/typed-list-view.hpp"
#include "ui/vlist/common/section-model.hpp"
#include "ui/vlist/common/vertical-list-model.hpp"

enum class ManageFallbackSection : std::uint8_t { Enabled, Available };

class ManageFallbackModel
    : public vicinae::ui::VerticalListModel<std::shared_ptr<RootItem>, ManageFallbackSection> {
  using ItemList = std::vector<std::shared_ptr<RootItem>>;

public:
  void setEnabled(const ItemList &list) { m_enabled = list; }
  void setAvailable(const ItemList &list) { m_available = list; }
  void requestUpdate() const { emit dataChanged(); }

protected:
  using S = ManageFallbackSection;

  virtual ItemData createItemData(const Item &item) const override {
    return {
        .title = item->displayName(),
        .icon = item->iconUrl(),
    };
  }

  virtual int sectionCount() const override { return 2; }
  virtual int sectionItemCount(ManageFallbackSection id) const override {
    return id == S::Enabled ? m_enabled.size() : m_available.size();
  }

  virtual Item sectionItemAt(ManageFallbackSection id, int itemIdx) const override {
    return id == S::Enabled ? m_enabled.at(itemIdx) : m_available.at(itemIdx);
  }

  virtual ManageFallbackSection sectionIdFromIndex(int idx) const override {
    return idx == 0 ? S::Enabled : S::Available;
  }

  virtual std::string_view sectionName(ManageFallbackSection id) const override {
    return id == S::Enabled ? "Enabled" : "Available";
  }

  virtual VListModel::StableID stableId(const Item &item) const override { return hash(item->uniqueId()); }

private:
  std::vector<std::shared_ptr<RootItem>> m_enabled;
  std::vector<std::shared_ptr<RootItem>> m_available;
};

class ManageFallbackView : public TypedListView<ManageFallbackModel> {
  std::unique_ptr<ActionPanelState> createActionPanel(const ItemType &item) const override {
    auto panel = std::make_unique<ListActionPanelState>();
    auto section = panel->createSection();
    auto it = std::ranges::find(m_fallbacks, item);

    if (it == m_fallbacks.end()) {
      section->addAction(new StaticAction("Enable fallback", ImageURL::builtin("redo"),
                                          [id = item->uniqueId()](ApplicationContext *ctx) {
                                            ctx->services->rootItemManager()->enableFallback(id);
                                          }));
    } else {
      section->addAction(new StaticAction("Disable fallback", ImageURL::builtin("redo"),
                                          [id = item->uniqueId()](ApplicationContext *ctx) {
                                            ctx->services->rootItemManager()->disableFallback(id);
                                          }));
    }

    return panel;
  }

  void handleRootSearch(const QString &text) {
    auto manager = context()->services->rootItemManager();
    auto results = manager->search(text);

    std::vector<std::shared_ptr<RootItem>> enabled;
    std::vector<std::shared_ptr<RootItem>> available;

    for (const RootItemManager::ScoredItem &scored : results) {
      auto item = scored.item.get().get();
      if (!item->isSuitableForFallback()) continue;
      auto meta = manager->itemMetadata(item->uniqueId());

      if (meta.fallback) {
        enabled.emplace_back(scored.item.get());
      } else {
        available.emplace_back(scored.item.get());
      }
    }

    std::ranges::stable_sort(enabled, [&](auto &&a, auto &&b) {
      int idxA = std::distance(std::ranges::find(m_fallbacks, a), m_fallbacks.begin());
      int idxB = std::distance(std::ranges::find(m_fallbacks, b), m_fallbacks.begin());
      return idxA > idxB;
    });

    m_model->setEnabled(enabled);
    m_model->setAvailable(available);
    m_model->requestUpdate();
    m_list->selectFirst();
  }

  void reloadSearch() {
    auto manager = context()->services->rootItemManager();
    m_fallbacks = manager->fallbackItems();

    if (!searchText().isEmpty()) {
      setSearchText("");
    } else {
      handleRootSearch(searchText());
    }
  }

  void textChanged(const QString &text) override { handleRootSearch(text); }

  void initialize() override {
    auto manager = context()->services->rootItemManager();
    TypedListView::initialize();
    setModel(new ManageFallbackModel);
    reloadSearch();

    connect(manager, &RootItemManager::fallbackEnabled, this, [this]() { reloadSearch(); });
    connect(manager, &RootItemManager::fallbackDisabled, this, [this]() { reloadSearch(); });
  }

  std::vector<std::shared_ptr<RootItem>> m_fallbacks;
};
