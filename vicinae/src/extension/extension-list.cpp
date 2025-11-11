#include "extension-list.hpp"
#include "fuzzy/weighted-fuzzy-scorer.hpp"
#include "layout.hpp"
#include "lib/fts_fuzzy.hpp"

class ExtensionListItem : public AbstractDefaultListItem {
public:
  const ListItemViewModel &model() const { return _item; }

  ExtensionListItem(const ListItemViewModel &model) : _item(model) {}

private:
  AccessoryList accessories() const {
    AccessoryList list;
    list.reserve(_item.accessories.size());
    for (const auto &accessory : _item.accessories) {
      list.emplace_back(accessory.toAccessory());
    }
    return list;
  }

  ItemData data() const override {
    return {
        .iconUrl = _item.icon, .name = _item.title, .subtitle = _item.subtitle, .accessories = accessories()};
  }

  QString generateId() const override { return _item.id; }

  ListItemViewModel _item;
};

ExtensionList::ExtensionList() {
  VStack().add(m_list).imbue(this);
  connect(m_list, &OmniList::selectionChanged, this, &ExtensionList::handleSelectionChanged);
  connect(m_list, &OmniList::itemActivated, this, &ExtensionList::handleItemActivated);
}

bool ExtensionList::selectUp() { return m_list->selectUp(); }
bool ExtensionList::selectDown() { return m_list->selectDown(); }
bool ExtensionList::selectHome() { return m_list->selectHome(); }
bool ExtensionList::selectEnd() { return m_list->selectEnd(); }
void ExtensionList::selectNext() { return m_list->selectNext(); }
void ExtensionList::activateCurrentSelection() const { m_list->activateCurrentSelection(); }

bool ExtensionList::empty() const { return m_list->virtualHeight() == 0; }

void ExtensionList::setModel(const std::vector<ListChild> &model, OmniList::SelectionPolicy selection) {
  m_model = model;
  render(selection);
}

void ExtensionList::setFilter(const QString &query) {
  if (m_filter == query) return;
  m_filter = query;
  render(OmniList::SelectFirst);
}

ListItemViewModel const *ExtensionList::selected() const {
  if (auto selected = m_list->selected()) {
    if (auto qualified = dynamic_cast<ExtensionListItem const *>(selected)) { return &qualified->model(); }
  }

  return nullptr;
}

int ExtensionList::computeItemScore(const ListItemViewModel &item, const std::string &query) {
  static const constexpr double TITLE_WEIGHT = 1;
  static const constexpr double SUBTITLE_WEIGHT = 0.6;
  static const constexpr double KEYWORD_WEIGHT = 0.3;
  fuzzy::WeightedScorer scorer;

  scorer.reserve(2 + item.keywords.size());
  scorer.add(item.title.toStdString(), TITLE_WEIGHT);
  scorer.add(item.subtitle.toStdString(), SUBTITLE_WEIGHT);
  for (const auto &kw : item.keywords) {
    scorer.add(kw.toStdString(), KEYWORD_WEIGHT);
  }
  return scorer.score(query);
}

void ExtensionList::render(OmniList::SelectionPolicy selectionPolicy) {
  std::string filter = m_filter.toStdString();
  std::vector<SectionGroup> groups;
  SectionGroup unamedGroup;

  groups.reserve(m_model.size());

  auto associateScore = [&](const ListItemViewModel &model) {
    return std::pair{&model, computeItemScore(model, filter)};
  };
  auto filterScore = [&](auto &&p) { return filter.empty() || p.second > 0; };

  auto handleSingleItem = [&](const ListItemViewModel &model) {
    if (auto p = associateScore(model); filterScore(p)) { unamedGroup.scoredItems.emplace_back(p); }
  };

  auto handleSection = [&](const ListSectionModel &model) {
    if (!unamedGroup.scoredItems.empty()) { groups.emplace_back(std::move(unamedGroup)); }
    SectionGroup grp;
    grp.section = &model;
    grp.scoredItems.reserve(model.children.size());
    auto candidates =
        model.children | std::views::transform(associateScore) | std::views::filter(filterScore);
    for (const auto &item : candidates) {
      grp.bestScore = std::max(grp.bestScore, item.second);
      grp.scoredItems.emplace_back(item);
    }
    groups.emplace_back(grp);
  };

  const auto visitor = overloads{handleSingleItem, handleSection};
  std::ranges::for_each(m_model, [&](auto &&item) { std::visit(visitor, item); });

  if (!unamedGroup.scoredItems.empty()) { groups.emplace_back(unamedGroup); }

  auto buildList = [&]() {
    // TODO: do not perform this sort if section order should be preserved
    std::ranges::stable_sort(groups, [](auto &&a, auto &&b) { return a.bestScore > b.bestScore; });
    for (SectionGroup &grp : groups) {
      auto &sec = m_list->addSection(grp.section ? grp.section->title : "");
      std::ranges::stable_sort(grp.scoredItems, [](auto &&p1, auto &&p2) { return p1.second > p2.second; });
      auto items = grp.scoredItems |
                   std::views::transform([](auto &&p) -> std::unique_ptr<OmniList::AbstractVirtualItem> {
                     return std::make_unique<ExtensionListItem>(*p.first);
                   });
      sec.addItems(items | std::ranges::to<std::vector>());
    }
  };

  m_list->updateModel(buildList, selectionPolicy);
}

void ExtensionList::handleSelectionChanged(const OmniList::AbstractVirtualItem *next,
                                           const OmniList::AbstractVirtualItem *previous) {
  if (!next) {
    emit selectionChanged(nullptr);
    return;
  }

  if (auto qualifiedNext = dynamic_cast<const ExtensionListItem *>(next)) {
    emit selectionChanged(&qualifiedNext->model());
  }
}

void ExtensionList::handleItemActivated(const OmniList::AbstractVirtualItem &item) {
  if (auto qualified = dynamic_cast<const ExtensionListItem *>(&item)) {
    emit itemActivated(qualified->model());
  }
}
