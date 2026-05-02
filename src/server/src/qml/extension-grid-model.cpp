#include "extension-grid-model.hpp"

#include <utility>
#include "common/types.hpp"
#include "fuzzy/fuzzy-searchable.hpp"
#include "theme.hpp"
#include "theme/theme-file.hpp"
#include "ui/image/url.hpp"
#include "view-utils.hpp"

template <> struct fuzzy::FuzzySearchable<GridItemViewModel> {
  static int score(const GridItemViewModel &item, std::string_view query) {
    std::vector<fuzzy::WeightedField> fields;
    fields.reserve(2 + item.keywords.size());
    fields.push_back({item.title, 1.0});
    fields.push_back({item.subtitle, 0.5});
    for (const auto &kw : item.keywords)
      fields.push_back({kw, 0.3});

    return fuzzy::scoreWeighted(fields, query);
  }
};

// --- ExtensionGridSection ---

ExtensionGridSection::ExtensionGridSection(std::string name, std::vector<GridItemViewModel> items,
                                           std::optional<int> columns, std::optional<double> aspectRatio,
                                           bool filtering, NotifyFn notify, SubmenuCache *cache,
                                           const std::optional<ActionPannelModel> *globalActions)
    : m_name(std::move(name)), m_items(std::move(items)), m_columns(columns), m_aspectRatio(aspectRatio),
      m_filtering(filtering), m_notify(std::move(notify)), m_cache(cache), m_globalActions(globalActions) {}

int ExtensionGridSection::count() const {
  if (m_filtering && !m_query.empty()) return static_cast<int>(m_filtered.size());
  return static_cast<int>(m_items.size());
}

void ExtensionGridSection::setFilter(std::string_view query) {
  m_query = std::string(query);
  if (!m_filtering) return;
  fuzzy::fuzzyFilter<GridItemViewModel>(std::span<const GridItemViewModel>(m_items), m_query, m_filtered);
}

void ExtensionGridSection::onSelected(int i) {
  if (m_onItemSelected) m_onItemSelected(itemAt(i));
}

const GridItemViewModel *ExtensionGridSection::itemAt(int i) const {
  if (m_filtering && !m_query.empty()) {
    if (i < 0 || std::cmp_greater_equal(i, m_filtered.size())) return nullptr;
    return &m_items[m_filtered[i].data];
  }
  if (i < 0 || std::cmp_greater_equal(i, m_items.size())) return nullptr;
  return &m_items[i];
}

std::unique_ptr<ActionPanelState> ExtensionGridSection::actionPanel(int i) const {
  if (auto *it = itemAt(i); it && it->actionPannel) {
    return ExtensionActionPanelBuilder::build(*it->actionPannel, m_notify, m_cache,
                                              ActionPanelState::ShortcutPreset::List);
  }
  if (m_globalActions && *m_globalActions) {
    return ExtensionActionPanelBuilder::build(**m_globalActions, m_notify, m_cache,
                                              ActionPanelState::ShortcutPreset::List);
  }
  return nullptr;
}

// --- ExtensionGridModel ---

ExtensionGridModel::ExtensionGridModel(NotifyFn notify, QObject *parent)
    : SectionGridModel(parent), m_notify(std::move(notify)) {}

void ExtensionGridModel::setExtensionData(const GridModel &model, bool resetSelection) {
  m_model = model;
  m_placeholder = QString::fromStdString(model.searchPlaceholderText);

  if (model.columns) { setColumns(*model.columns); }
  if (model.aspectRatio > 0.0) { setAspectRatio(model.aspectRatio); }
  if (m_fit != model.fit) {
    m_fit = model.fit;
    emit fitChanged();
  }

  double newInset;
  switch (model.inset) {
  case GridInset::None:
    newInset = 0.05;
    break;
  case GridInset::Small:
    newInset = 0.10;
    break;
  case GridInset::Medium:
    newInset = 0.15;
    break;
  case GridInset::Large:
    newInset = 0.25;
    break;
  default:
    newInset = 0.10;
    break;
  }
  if (m_fit == ObjectFit::Fill) newInset = 0.0;
  if (!qFuzzyCompare(m_inset, newInset)) {
    m_inset = newInset;
    emit insetChanged();
  }

  rebuildFromSections(resetSelection);

  emit emptyViewChanged();
}

void ExtensionGridModel::rebuildFromSections(bool resetSelection) {
  int const prevSection = selectedSection();
  int const prevItem = selectedItem();

  clearSources();
  m_ownedSections.clear();

  std::string freeItems;
  std::vector<GridItemViewModel> freeBuf;

  auto flushFree = [&]() {
    if (freeBuf.empty()) return;
    auto section = std::make_unique<ExtensionGridSection>(std::move(freeItems), std::move(freeBuf),
                                                          std::nullopt, std::nullopt, m_model.filtering,
                                                          m_notify, &m_submenuCache, &m_model.actions);
    section->setOnItemSelected([this](const GridItemViewModel *item) {
      if (auto handler = m_model.onSelectionChanged) {
        if (item) { m_notify(handler->c_str(), {item->id.c_str()}); }
      }
    });
    addSource(section.get());
    m_ownedSections.push_back(std::move(section));
    freeBuf = {};
    freeItems = {};
  };

  for (const auto &child : m_model.items) {
    if (auto item = std::get_if<GridItemViewModel>(&child)) {
      freeBuf.push_back(*item);
    } else if (auto sec = std::get_if<GridSectionModel>(&child)) {
      flushFree();
      auto section = std::make_unique<ExtensionGridSection>(sec->title, sec->children, sec->columns,
                                                            sec->aspectRatio, m_model.filtering, m_notify,
                                                            &m_submenuCache, &m_model.actions);
      section->setOnItemSelected([this](const GridItemViewModel *item) {
        if (auto handler = m_model.onSelectionChanged) {
          if (item) { m_notify(handler->c_str(), {item->id.c_str()}); }
        }
      });
      addSource(section.get());
      m_ownedSections.push_back(std::move(section));
    }
  }
  flushFree();

  setSelectFirstOnReset(resetSelection);
  if (m_model.filtering && !m_filter.isEmpty()) {
    SectionGridModel::setFilter(m_filter);
  } else {
    rebuild();
  }
  setSelectFirstOnReset(false);

  if (!resetSelection) {
    bool const prevValid = prevSection >= 0 && std::cmp_less(prevSection, m_ownedSections.size()) &&
                           prevItem >= 0 && prevItem < m_ownedSections[prevSection]->count();

    if (prevValid) {
      if (prevSection == selectedSection() && prevItem == selectedItem()) {
        refreshActionPanel();
        emit selectionChanged();
      } else {
        select(prevSection, prevItem);
      }
    } else {
      selectFirst();
    }
  } else {
    selectFirst();
  }

  ++m_dataRevision;
  emit dataRevisionChanged();
}

void ExtensionGridModel::setFilter(const QString &text) {
  m_filter = text;
  if (m_model.filtering) {
    setSelectFirstOnReset(true);
    SectionGridModel::setFilter(text);
    setSelectFirstOnReset(false);
    selectFirst();
  }
}

QString ExtensionGridModel::searchPlaceholder() const {
  return m_placeholder.isEmpty() ? QStringLiteral("Search...") : m_placeholder;
}

const GridItemViewModel *ExtensionGridModel::resolveItem(int section, int item) const {
  int sourceIdx, itemIdx;
  if (!resolveSelection(section, item, sourceIdx, itemIdx)) return nullptr;
  return m_ownedSections[sourceIdx]->itemAt(itemIdx);
}

QString ExtensionGridModel::cellTitle(int section, int item) const {
  if (auto *it = resolveItem(section, item)) return QString::fromStdString(it->title);
  return {};
}

QString ExtensionGridModel::cellIcon(int section, int item) const {
  if (auto *it = resolveItem(section, item)) {
    if (auto img = std::get_if<ImageLikeModel>(&it->content)) { return qml::imageSourceFor(ImageURL(*img)); }
  }
  return {};
}

QString ExtensionGridModel::cellSubtitle(int section, int item) const {
  if (auto *it = resolveItem(section, item)) return QString::fromStdString(it->subtitle);
  return {};
}

QString ExtensionGridModel::cellTooltip(int section, int item) const {
  if (auto *it = resolveItem(section, item)) {
    if (it->tooltip) return QString::fromStdString(*it->tooltip);
  }
  return {};
}

QString ExtensionGridModel::cellColor(int section, int item) const {
  if (auto *it = resolveItem(section, item)) {
    if (auto *color = std::get_if<ColorLike>(&it->content)) {
      const auto &theme = ThemeService::instance().theme();
      return std::visit(
          overloads{
              [](const QColor &c) -> QString { return c.name(QColor::HexArgb); },
              [](const QString &c) -> QString { return c; },
              [&](const SemanticColor &c) -> QString { return theme.resolve(c).name(QColor::HexArgb); },
              [&](const DynamicColor &c) -> QString { return theme.isLight() ? c.light : c.dark; },
          },
          *color);
    }
  }
  return {};
}

QString ExtensionGridModel::emptyTitle() const {
  if (m_model.emptyView) return m_model.emptyView->title;
  return QStringLiteral("No results");
}

QString ExtensionGridModel::emptyDescription() const {
  if (m_model.emptyView) return m_model.emptyView->description;
  return {};
}

QString ExtensionGridModel::emptyIcon() const {
  if (m_model.emptyView && m_model.emptyView->icon)
    return qml::imageSourceFor(ImageURL(*m_model.emptyView->icon));
  return {};
}

void ExtensionGridModel::onSelectionCleared() {
  std::unique_ptr<ActionPanelState> panel;

  if (m_model.emptyView && m_model.emptyView->actions) {
    panel = ExtensionActionPanelBuilder::build(*m_model.emptyView->actions, m_notify, &m_submenuCache,
                                               ActionPanelState::ShortcutPreset::List);
  } else if (m_model.actions) {
    panel = ExtensionActionPanelBuilder::build(*m_model.actions, m_notify, &m_submenuCache,
                                               ActionPanelState::ShortcutPreset::List);
  }

  if (panel) {
    panel->finalize();
    scope().setActions(std::move(panel));
  } else {
    SectionGridModel::onSelectionCleared();
  }
}
