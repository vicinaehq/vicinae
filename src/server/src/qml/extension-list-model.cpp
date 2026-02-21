#include "extension-list-model.hpp"
#include "navigation-controller.hpp"
#include "view-utils.hpp"
#include "ui/image/url.hpp"
#include <algorithm>

ExtensionListModel::ExtensionListModel(NotifyFn notify, QObject *parent)
    : CommandListModel(parent), m_notify(std::move(notify)) {}

void ExtensionListModel::setExtensionData(const ListModel &model, bool resetSelection) {
  m_model = model;
  m_placeholder = QString::fromStdString(model.searchPlaceholderText);

  m_sections.clear();

  Section freeSection;
  freeSection.name = "";

  for (const auto &child : model.items) {
    if (auto item = std::get_if<ListItemViewModel>(&child)) {
      freeSection.items.push_back(*item);
    } else if (auto section = std::get_if<ListSectionModel>(&child)) {
      if (!freeSection.items.empty()) {
        m_sections.push_back(std::move(freeSection));
        freeSection = Section{.name = "", .items = {}};
      }

      Section sec;
      sec.name = section->title;
      sec.items = section->children;
      m_sections.push_back(std::move(sec));
    }
  }

  if (!freeSection.items.empty()) {
    m_sections.push_back(std::move(freeSection));
  }

  if (!resetSelection) setSelectFirstOnReset(false);

  if (m_model.filtering && !m_filter.isEmpty()) {
    setFilter(m_filter);
  } else {
    m_filteredSections.clear();
    rebuildFromModel();
  }

  if (!resetSelection) {
    setSelectFirstOnReset(true);
    invalidateSelection();
    refreshActionPanel();
  }

  emit emptyViewChanged();
  emit detailChanged();
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
    return imageSourceFor(ImageURL(*m_model.emptyView->icon));
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

void ExtensionListModel::rebuildFromModel() {
  const auto &sections = activeSections();

  std::vector<SectionInfo> infos;
  infos.reserve(sections.size());
  bool hasItems = false;
  for (const auto &sec : sections) {
    infos.push_back({QString::fromStdString(sec.name), static_cast<int>(sec.items.size())});
    if (!sec.items.empty()) hasItems = true;
  }
  setSections(infos);

  if (!hasItems) { onSelectionCleared(); }
}

const std::vector<ExtensionListModel::Section> &ExtensionListModel::activeSections() const {
  if (m_model.filtering && !m_filter.isEmpty()) { return m_filteredSections; }
  return m_sections;
}

bool ExtensionListModel::matchesFilter(const ListItemViewModel &item, const QString &filter) const {
  if (filter.isEmpty()) return true;

  if (QString::fromStdString(item.title).contains(filter, Qt::CaseInsensitive)) return true;
  if (QString::fromStdString(item.subtitle).contains(filter, Qt::CaseInsensitive)) return true;

  for (const auto &kw : item.keywords) {
    if (QString::fromStdString(kw).contains(filter, Qt::CaseInsensitive)) return true;
  }

  return false;
}

void ExtensionListModel::setFilter(const QString &text) {
  m_filter = text;

  if (m_model.filtering) {
    m_filteredSections.clear();
    for (const auto &sec : m_sections) {
      Section filtered;
      filtered.name = sec.name;
      for (const auto &item : sec.items) {
        if (matchesFilter(item, text)) { filtered.items.push_back(item); }
      }
      m_filteredSections.push_back(std::move(filtered));
    }
    rebuildFromModel();
  }
}

QString ExtensionListModel::searchPlaceholder() const {
  return m_placeholder.isEmpty() ? QStringLiteral("Search...") : m_placeholder;
}

const ListItemViewModel *ExtensionListModel::itemAt(int section, int item) const {
  const auto &sections = activeSections();
  if (section < 0 || section >= static_cast<int>(sections.size())) return nullptr;
  const auto &sec = sections[section];
  if (item < 0 || item >= static_cast<int>(sec.items.size())) return nullptr;
  return &sec.items[item];
}

QString ExtensionListModel::itemTitle(int section, int item) const {
  if (auto *it = itemAt(section, item)) return QString::fromStdString(it->title);
  return {};
}

QString ExtensionListModel::itemSubtitle(int section, int item) const {
  if (auto *it = itemAt(section, item)) return QString::fromStdString(it->subtitle);
  return {};
}

QString ExtensionListModel::itemIconSource(int section, int item) const {
  if (auto *it = itemAt(section, item)) {
    if (it->icon) { return imageSourceFor(ImageURL(*it->icon)); }
  }
  return {};
}

QVariant ExtensionListModel::itemAccessory(int section, int item) const {
  if (auto *it = itemAt(section, item)) {
    if (!it->accessories.empty()) { return qml::accessoriesToVariantList(it->accessories); }
  }
  return {};
}

std::unique_ptr<ActionPanelState> ExtensionListModel::createActionPanel(int section, int item) const {
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

void ExtensionListModel::onSelectionCleared() {
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
    ctx()->navigation->setActions(std::move(panel));
  } else {
    CommandListModel::onSelectionCleared();
  }
}

void ExtensionListModel::onItemSelected(int section, int item) {
  auto *it = itemAt(section, item);
  m_currentDetail = it ? it->detail : std::nullopt;
  emit detailChanged();

  if (auto handler = m_model.onSelectionChanged) {
    if (it) { m_notify(handler->c_str(), {it->id.c_str()}); }
  }
}
