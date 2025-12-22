#pragma once
#include "root-search/extensions/extension-root-provider.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include "ui/image/url.hpp"
#include "ui/views/typed-list-view.hpp"
#include "ui/vlist/common/vertical-list-model.hpp"

enum class ProviderSearchSectionType { Main };

class ProviderSearchModel : public vicinae::ui::VerticalListModel<const RootItem *> {
public:
  ProviderSearchModel(RootItemManager *manager) : m_manager(manager) {}

  void setItems(std::vector<RootItemManager::ScoredItem> items) {
    m_items = items;
    emit dataChanged();
  }

protected:
  int sectionCount() const override { return 1; }
  int sectionIdFromIndex(int idx) const override { return idx; }

  ItemData createItemData(const RootItem *const &item) const override {
    return {.title = item->displayName(),
            .subtitle = item->subtitle(),
            .icon = item->iconUrl(),
            .isActive = item->isActive()};
  }

  int sectionItemCount(int id) const override { return m_items.size(); }

  std::string_view sectionName(int id) const override { return "Results ({count})"; }

  const RootItem *sectionItemAt(int id, int itemIdx) const override {
    return m_items.at(itemIdx).item.get().get();
  }

  StableID stableId(const Item &item) const override { return std::hash<EntrypointId>{}(item->uniqueId()); }

private:
  std::string query;
  std::vector<RootItemManager::ScoredItem> m_items;

  RootItemManager *m_manager = nullptr;
};

class ProviderSearchView : public TypedListView<ProviderSearchModel> {
public:
  ProviderSearchView(const ExtensionRootProvider &provider)
      : m_displayName(provider.repository()->displayName()), m_icon(provider.icon()),
        m_provider(provider.uniqueId()) {}

  QString initialSearchPlaceholderText() const override { return QString("Search %1").arg(m_displayName); }

  QString initialNavigationTitle() const override { return m_displayName; }

  ImageURL initialNavigationIcon() const override { return m_icon; }

  std::unique_ptr<ActionPanelState> createActionPanel(const ItemType &item) const override {
    return item->newActionPanel(context(), m_manager->itemMetadata(item->uniqueId()));
  }

  std::unique_ptr<CompleterData> createCompleter(const ItemType &item) const override {
    ArgumentList args = item->arguments();

    if (args.empty()) return nullptr;

    auto cmpl = std::make_unique<CompleterData>();

    cmpl->arguments = args;
    cmpl->iconUrl = item->iconUrl();

    return cmpl;
  }

  bool tryAliasFastTrack() {
    auto selection = m_list->currentSelection();

    if (!selection) return false;

    auto item = m_model->fromIndex(*selection).value();

    if (!item->supportsAliasSpaceShortcut()) return false;

    auto alias = m_manager->itemMetadata(item->uniqueId()).alias;

    return alias && alias->starts_with(searchText().toStdString());
  }

  bool inputFilter(QKeyEvent *event) override {
    if (event->modifiers().toInt() == 0) {
      switch (event->key()) {
      case Qt::Key_Space:
        return tryAliasFastTrack();
      }
    }
    return TypedListView::inputFilter(event);
  }

  void initialize() override {
    TypedListView::initialize();
    m_manager = context()->services->rootItemManager();
    m_model = new ProviderSearchModel(m_manager);
    setModel(m_model);
    textChanged("");
    connect(m_manager, &RootItemManager::itemsChanged, this, [this]() { setSearchText(searchText()); });
  }

  void textChanged(const QString &text) override {
    auto results = m_manager->search(text, {.providerId = m_provider.toStdString()});
    model()->setItems(results);
    m_list->selectFirst();
  }

private:
  QString m_provider;
  QString m_displayName;
  ImageURL m_icon;
  RootItemManager *m_manager = nullptr;
};
