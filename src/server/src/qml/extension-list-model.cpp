#include "extension-list-model.hpp"

#include <utility>
#include "fuzzy/fuzzy-searchable.hpp"
#include "view-utils.hpp"
#include "ui/image/url.hpp"

template <> struct fuzzy::FuzzySearchable<ListItemViewModel> {
  static int score(const ListItemViewModel &item, std::string_view query) {
    std::vector<fuzzy::WeightedField> fields;
    fields.reserve(2 + item.keywords.size());
    fields.push_back({item.title, 1.0});
    fields.push_back({item.subtitle, 0.5});
    for (const auto &kw : item.keywords)
      fields.push_back({kw, 0.3});

    return fuzzy::scoreWeighted(fields, query);
  }
};

// --- ExtensionListSection ---

ExtensionListSection::ExtensionListSection(std::string name, std::vector<ListItemViewModel> items,
                                           bool filtering, NotifyFn notify, SubmenuCache *cache,
                                           const std::optional<ActionPannelModel> *globalActions)
    : m_name(std::move(name)), m_items(std::move(items)), m_filtering(filtering), m_notify(std::move(notify)),
      m_cache(cache), m_globalActions(globalActions) {}

int ExtensionListSection::count() const {
  if (m_filtering && !m_query.empty()) return static_cast<int>(m_filtered.size());
  return static_cast<int>(m_items.size());
}

const ListItemViewModel &ExtensionListSection::itemAt(int i) const {
  if (m_filtering && !m_query.empty()) return m_items[m_filtered[i].data];
  return m_items[i];
}

void ExtensionListSection::setFilter(std::string_view query) {
  m_query = std::string(query);
  if (!m_filtering) return;
  fuzzy::fuzzyFilter<ListItemViewModel>(std::span<const ListItemViewModel>(m_items), m_query, m_filtered);
}

void ExtensionListSection::onSelected(int i) {
  if (m_onItemSelected) m_onItemSelected(&itemAt(i));
}

QString ExtensionListSection::itemTitle(int i) const { return QString::fromStdString(itemAt(i).title); }

QString ExtensionListSection::itemSubtitle(int i) const { return QString::fromStdString(itemAt(i).subtitle); }

QString ExtensionListSection::itemIconSource(int i) const {
  const auto &item = itemAt(i);
  if (item.icon) return imageSourceFor(ImageURL(*item.icon));
  return {};
}

QVariantList ExtensionListSection::itemAccessories(int i) const {
  const auto &item = itemAt(i);
  if (!item.accessories.empty()) return qml::accessoriesToVariantList(item.accessories);
  return {};
}

std::unique_ptr<ActionPanelState> ExtensionListSection::actionPanel(int i) const {
  const auto &item = itemAt(i);
  if (item.actionPannel) {
    return ExtensionActionPanelBuilder::build(*item.actionPannel, m_notify, m_cache,
                                              ActionPanelState::ShortcutPreset::List);
  }
  if (m_globalActions && *m_globalActions) {
    return ExtensionActionPanelBuilder::build(**m_globalActions, m_notify, m_cache,
                                              ActionPanelState::ShortcutPreset::List);
  }
  return nullptr;
}

// --- ExtensionListModel ---

ExtensionListModel::ExtensionListModel(NotifyFn notify, SubmenuCache *cache, QObject *parent)
    : SectionListModel(parent), m_notify(std::move(notify)), m_submenuCache(cache) {}

void ExtensionListModel::setExtensionData(const ListModel &model, bool resetSelection) {
  m_model = model;
  m_placeholder = QString::fromStdString(model.searchPlaceholderText);

  clearSources();
  m_ownedSections.clear();

  std::string freeItems;
  std::vector<ListItemViewModel> freeBuf;

  auto flushFree = [&]() {
    if (freeBuf.empty()) return;
    auto section =
        std::make_unique<ExtensionListSection>(std::move(freeItems), std::move(freeBuf), model.filtering,
                                               m_notify, m_submenuCache, &m_model.actions);
    section->setOnItemSelected([this](const ListItemViewModel *item) { handleItemSelected(item); });
    addSource(section.get());
    m_ownedSections.push_back(std::move(section));
    freeBuf = {};
    freeItems = {};
  };

  for (const auto &child : model.items) {
    if (auto item = std::get_if<ListItemViewModel>(&child)) {
      freeBuf.push_back(*item);
    } else if (auto sec = std::get_if<ListSectionModel>(&child)) {
      flushFree();
      auto section = std::make_unique<ExtensionListSection>(sec->title, sec->children, model.filtering,
                                                            m_notify, m_submenuCache, &m_model.actions);
      section->setOnItemSelected([this](const ListItemViewModel *item) { handleItemSelected(item); });
      addSource(section.get());
      m_ownedSections.push_back(std::move(section));
    }
  }
  flushFree();

  if (!resetSelection) setSelectFirstOnReset(false);

  if (m_model.filtering && !m_filter.isEmpty()) {
    SectionListModel::setFilter(m_filter);
  } else {
    rebuild();
  }

  if (!resetSelection) {
    setSelectFirstOnReset(true);
    refreshActionPanel();
  }

  emit emptyViewChanged();
  emit detailChanged();
}

void ExtensionListModel::setFilter(const QString &text) {
  m_filter = text;
  if (m_model.filtering) { SectionListModel::setFilter(text); }
}

QString ExtensionListModel::searchPlaceholder() const {
  return m_placeholder.isEmpty() ? QStringLiteral("Search...") : m_placeholder;
}

QString ExtensionListModel::emptyTitle() const {
  if (m_model.emptyView) return m_model.emptyView->title;
  return QStringLiteral("No results");
}

QString ExtensionListModel::emptyDescription() const {
  if (m_model.emptyView) return m_model.emptyView->description;
  return {};
}

QString ExtensionListModel::emptyIcon() const {
  if (m_model.emptyView && m_model.emptyView->icon)
    return qml::imageSourceFor(ImageURL(*m_model.emptyView->icon));
  return {};
}

bool ExtensionListModel::isShowingDetail() const { return m_model.isShowingDetail; }

bool ExtensionListModel::hasDetail() const { return m_currentDetail.has_value(); }

QString ExtensionListModel::detailMarkdown() const {
  if (m_currentDetail && m_currentDetail->markdown) return *m_currentDetail->markdown;
  return {};
}

QVariantList ExtensionListModel::detailMetadata() const {
  if (!m_currentDetail) return {};
  return qml::metadataToVariantList(m_currentDetail->metadata);
}

void ExtensionListModel::onSelectionCleared() {
  std::unique_ptr<ActionPanelState> panel;

  if (m_model.emptyView && m_model.emptyView->actions) {
    panel = ExtensionActionPanelBuilder::build(*m_model.emptyView->actions, m_notify, m_submenuCache,
                                               ActionPanelState::ShortcutPreset::List);
  } else if (m_model.actions) {
    panel = ExtensionActionPanelBuilder::build(*m_model.actions, m_notify, m_submenuCache,
                                               ActionPanelState::ShortcutPreset::List);
  }

  if (panel) {
    panel->finalize();
    scope().setActions(std::move(panel));
  } else {
    SectionListModel::onSelectionCleared();
  }
}

void ExtensionListModel::handleItemSelected(const ListItemViewModel *item) {
  m_currentDetail = item ? item->detail : std::nullopt;
  emit detailChanged();

  if (auto handler = m_model.onSelectionChanged) {
    if (item) { m_notify(handler->c_str(), {item->id.c_str()}); }
  }
}
