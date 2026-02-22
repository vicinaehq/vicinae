#include "extension-grid-model.hpp"
#include "common/types.hpp"
#include "navigation-controller.hpp"
#include "theme.hpp"
#include "theme/theme-file.hpp"
#include "ui/image/url.hpp"
#include <algorithm>

ExtensionGridModel::ExtensionGridModel(NotifyFn notify, QObject *parent)
    : CommandGridModel(parent), m_notify(std::move(notify)) {}

void ExtensionGridModel::setExtensionData(const GridModel &model, bool resetSelection) {
  m_model = model;
  m_placeholder = QString::fromStdString(model.searchPlaceholderText);

  // Set grid-level defaults from the model
  if (model.columns) { setColumns(*model.columns); }
  if (model.aspectRatio > 0.0) { setAspectRatio(model.aspectRatio); }
  if (m_fit != model.fit) {
    m_fit = model.fit;
    emit fitChanged();
  }

  // Map Inset enum to ratio: None≈0.05, Small=0.10, Medium=0.15, Large=0.25
  double newInset;
  switch (model.inset) {
  case GridInset::None: newInset = 0.05; break;
  case GridInset::Small: newInset = 0.10; break;
  case GridInset::Medium: newInset = 0.15; break;
  case GridInset::Large: newInset = 0.25; break;
  default: newInset = 0.10; break;
  }
  if (!qFuzzyCompare(m_inset, newInset)) {
    m_inset = newInset;
    emit insetChanged();
  }

  m_sections.clear();

  Section freeSection;
  freeSection.name = "";

  for (const auto &child : model.items) {
    if (auto item = std::get_if<GridItemViewModel>(&child)) {
      freeSection.items.push_back(*item);
    } else if (auto section = std::get_if<GridSectionModel>(&child)) {
      if (!freeSection.items.empty()) {
        m_sections.push_back(std::move(freeSection));
        freeSection = Section{.name = "", .items = {}};
      }

      Section sec;
      sec.name = section->title;
      sec.aspectRatio = section->aspectRatio;
      sec.columns = section->columns;
      sec.items = section->children;
      m_sections.push_back(std::move(sec));
    }
  }

  if (!freeSection.items.empty()) { m_sections.push_back(std::move(freeSection)); }

  if (m_model.filtering && !m_filter.isEmpty()) {
    reapplyFilter();
  } else {
    m_filteredSections.clear();
  }
  rebuildFromModel(resetSelection);

  emit emptyViewChanged();
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
    return imageSourceFor(ImageURL(*m_model.emptyView->icon));
  return {};
}

void ExtensionGridModel::rebuildFromModel(bool resetSelection) {
  const auto &sections = activeSections();

  int prevSection = selectedSection();
  int prevItem = selectedItem();

  std::vector<SectionInfo> infos;
  infos.reserve(sections.size());
  for (const auto &sec : sections) {
    infos.push_back({QString::fromStdString(sec.name), static_cast<int>(sec.items.size()), sec.columns, sec.aspectRatio});
  }

  setSelectFirstOnReset(resetSelection);
  setSections(infos);
  setSelectFirstOnReset(false);

  if (!resetSelection) {
    bool prevValid = prevSection >= 0
        && prevSection < static_cast<int>(sections.size())
        && prevItem >= 0
        && prevItem < static_cast<int>(sections[prevSection].items.size());

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

const std::vector<ExtensionGridModel::Section> &ExtensionGridModel::activeSections() const {
  if (m_model.filtering && !m_filter.isEmpty()) { return m_filteredSections; }
  return m_sections;
}

bool ExtensionGridModel::matchesFilter(const GridItemViewModel &item, const QString &filter) const {
  if (filter.isEmpty()) return true;

  if (QString::fromStdString(item.title).contains(filter, Qt::CaseInsensitive)) return true;
  if (QString::fromStdString(item.subtitle).contains(filter, Qt::CaseInsensitive)) return true;

  for (const auto &kw : item.keywords) {
    if (QString::fromStdString(kw).contains(filter, Qt::CaseInsensitive)) return true;
  }

  return false;
}

void ExtensionGridModel::reapplyFilter() {
  m_filteredSections.clear();
  for (const auto &sec : m_sections) {
    Section filtered;
    filtered.name = sec.name;
    filtered.aspectRatio = sec.aspectRatio;
    filtered.columns = sec.columns;
    for (const auto &item : sec.items) {
      if (matchesFilter(item, m_filter)) { filtered.items.push_back(item); }
    }
    m_filteredSections.push_back(std::move(filtered));
  }
}

void ExtensionGridModel::setFilter(const QString &text) {
  m_filter = text;

  if (m_model.filtering) {
    reapplyFilter();
    rebuildFromModel(true);
  }
}

QString ExtensionGridModel::searchPlaceholder() const {
  return m_placeholder.isEmpty() ? QStringLiteral("Search...") : m_placeholder;
}

const GridItemViewModel *ExtensionGridModel::itemAt(int section, int item) const {
  const auto &sections = activeSections();
  if (section < 0 || section >= static_cast<int>(sections.size())) return nullptr;
  const auto &sec = sections[section];
  if (item < 0 || item >= static_cast<int>(sec.items.size())) return nullptr;
  return &sec.items[item];
}

QString ExtensionGridModel::cellTitle(int section, int item) const {
  if (auto *it = itemAt(section, item)) return QString::fromStdString(it->title);
  return {};
}

QString ExtensionGridModel::cellIcon(int section, int item) const {
  if (auto *it = itemAt(section, item)) {
    if (auto img = std::get_if<ImageLikeModel>(&it->content)) {
      return imageSourceFor(ImageURL(*img));
    }
    // ColorLike content — no icon URL
  }
  return {};
}

QString ExtensionGridModel::cellSubtitle(int section, int item) const {
  if (auto *it = itemAt(section, item)) return QString::fromStdString(it->subtitle);
  return {};
}

QString ExtensionGridModel::cellTooltip(int section, int item) const {
  if (auto *it = itemAt(section, item)) {
    if (it->tooltip) return QString::fromStdString(*it->tooltip);
  }
  return {};
}

QString ExtensionGridModel::cellColor(int section, int item) const {
  if (auto *it = itemAt(section, item)) {
    if (auto *color = std::get_if<ColorLike>(&it->content)) {
      const auto &theme = ThemeService::instance().theme();
      return std::visit(overloads{
        [](const QColor &c) -> QString { return c.name(QColor::HexArgb); },
        [](const QString &c) -> QString { return c; },
        [&](const SemanticColor &c) -> QString { return theme.resolve(c).name(QColor::HexArgb); },
        [&](const DynamicColor &c) -> QString { return theme.isLight() ? c.light : c.dark; },
      }, *color);
    }
  }
  return {};
}

std::unique_ptr<ActionPanelState> ExtensionGridModel::createActionPanel(int section, int item) const {
  if (auto *it = itemAt(section, item)) {
    if (it->actionPannel) {
      return ExtensionActionPanelBuilder::build(*it->actionPannel, m_notify, &m_submenuCache,
                                                ActionPanelState::ShortcutPreset::List);
    }
  }

  if (m_model.actions) {
    return ExtensionActionPanelBuilder::build(*m_model.actions, m_notify, &m_submenuCache,
                                              ActionPanelState::ShortcutPreset::List);
  }

  return nullptr;
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
    CommandGridModel::onSelectionCleared();
  }
}

void ExtensionGridModel::onItemSelected(int section, int item) {
  if (auto handler = m_model.onSelectionChanged) {
    if (auto *it = itemAt(section, item)) { m_notify(handler->c_str(), {it->id.c_str()}); }
  }
}
