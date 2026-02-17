#include "qml-extension-list-model.hpp"
#include "navigation-controller.hpp"
#include "qml-utils.hpp"
#include "ui/image/url.hpp"
#include <algorithm>

QmlExtensionListModel::QmlExtensionListModel(NotifyFn notify, QObject *parent)
    : QmlCommandListModel(parent), m_notify(std::move(notify)) {}

void QmlExtensionListModel::setExtensionData(const ListModel &model) {
  m_model = model;
  m_placeholder = QString::fromStdString(model.searchPlaceholderText);

  // Convert ListModel::items (flat items + section items) into sections
  m_sections.clear();

  Section freeSection;
  freeSection.name = "";

  for (const auto &child : model.items) {
    if (auto item = std::get_if<ListItemViewModel>(&child)) {
      freeSection.items.push_back(*item);
    } else if (auto section = std::get_if<ListSectionModel>(&child)) {
      // Flush any free items into their own section first
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

  // Flush remaining free items
  if (!freeSection.items.empty()) {
    // If there are no named sections, use an empty section name
    // If there are named sections, create a section with empty name
    m_sections.push_back(std::move(freeSection));
  }

  // Re-apply active filter against the new sections, or just rebuild.
  // The caller (view host) controls selectFirstOnReset based on whether
  // this update is a response to a search text change or just a data refresh.
  if (m_model.filtering && !m_filter.isEmpty()) {
    setFilter(m_filter);
  } else {
    m_filteredSections.clear();
    rebuildFromModel();
  }

  emit emptyViewChanged();
  emit detailChanged();
}

QString QmlExtensionListModel::emptyTitle() const {
  if (m_model.emptyView) return m_model.emptyView->title;
  return QStringLiteral("No results");
}

QString QmlExtensionListModel::emptyDescription() const {
  if (m_model.emptyView) return m_model.emptyView->description;
  return {};
}

QString QmlExtensionListModel::emptyIcon() const {
  if (m_model.emptyView && m_model.emptyView->icon)
    return imageSourceFor(ImageURL(*m_model.emptyView->icon));
  return {};
}

bool QmlExtensionListModel::isShowingDetail() const { return m_model.isShowingDetail; }

bool QmlExtensionListModel::hasDetail() const { return m_currentDetail.has_value(); }

QString QmlExtensionListModel::detailMarkdown() const {
  if (m_currentDetail && m_currentDetail->markdown) return *m_currentDetail->markdown;
  return {};
}

QVariantList QmlExtensionListModel::detailMetadata() const {
  if (!m_currentDetail) return {};
  return qml::metadataToVariantList(m_currentDetail->metadata);
}

void QmlExtensionListModel::rebuildFromModel() {
  const auto &sections = activeSections();

  std::vector<SectionInfo> infos;
  infos.reserve(sections.size());
  for (const auto &sec : sections) {
    infos.push_back({QString::fromStdString(sec.name), static_cast<int>(sec.items.size())});
  }
  setSections(infos);
}

const std::vector<QmlExtensionListModel::Section> &QmlExtensionListModel::activeSections() const {
  if (m_model.filtering && !m_filter.isEmpty()) { return m_filteredSections; }
  return m_sections;
}

bool QmlExtensionListModel::matchesFilter(const ListItemViewModel &item, const QString &filter) const {
  if (filter.isEmpty()) return true;

  if (QString::fromStdString(item.title).contains(filter, Qt::CaseInsensitive)) return true;
  if (QString::fromStdString(item.subtitle).contains(filter, Qt::CaseInsensitive)) return true;

  for (const auto &kw : item.keywords) {
    if (QString::fromStdString(kw).contains(filter, Qt::CaseInsensitive)) return true;
  }

  return false;
}

void QmlExtensionListModel::setFilter(const QString &text) {
  m_filter = text;

  if (m_model.filtering) {
    // Client-side filtering
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

QString QmlExtensionListModel::searchPlaceholder() const {
  return m_placeholder.isEmpty() ? QStringLiteral("Search...") : m_placeholder;
}

const ListItemViewModel *QmlExtensionListModel::itemAt(int section, int item) const {
  const auto &sections = activeSections();
  if (section < 0 || section >= static_cast<int>(sections.size())) return nullptr;
  const auto &sec = sections[section];
  if (item < 0 || item >= static_cast<int>(sec.items.size())) return nullptr;
  return &sec.items[item];
}

QString QmlExtensionListModel::itemTitle(int section, int item) const {
  if (auto *it = itemAt(section, item)) return QString::fromStdString(it->title);
  return {};
}

QString QmlExtensionListModel::itemSubtitle(int section, int item) const {
  if (auto *it = itemAt(section, item)) return QString::fromStdString(it->subtitle);
  return {};
}

QString QmlExtensionListModel::itemIconSource(int section, int item) const {
  if (auto *it = itemAt(section, item)) {
    if (it->icon) { return imageSourceFor(ImageURL(*it->icon)); }
  }
  return {};
}

QString QmlExtensionListModel::itemAccessory(int section, int item) const {
  if (auto *it = itemAt(section, item)) {
    if (!it->accessories.empty()) {
      const auto &acc = it->accessories.front();
      if (auto tag = std::get_if<AccessoryModel::Tag>(&acc.data)) { return tag->value; }
      if (auto text = std::get_if<AccessoryModel::Text>(&acc.data)) { return text->value; }
    }
  }
  return {};
}

std::unique_ptr<ActionPanelState> QmlExtensionListModel::createActionPanel(int section, int item) const {
  if (auto *it = itemAt(section, item)) {
    if (it->actionPannel) {
      return ExtensionActionPanelBuilder::build(*it->actionPannel, m_notify, &m_submenuCache,
                                                ActionPanelState::ShortcutPreset::List);
    }
  }

  // Fall back to view-level actions
  if (m_model.actions) {
    return ExtensionActionPanelBuilder::build(*m_model.actions, m_notify, &m_submenuCache,
                                              ActionPanelState::ShortcutPreset::List);
  }

  return nullptr;
}

void QmlExtensionListModel::onItemSelected(int section, int item) {
  auto *it = itemAt(section, item);
  m_currentDetail = it ? it->detail : std::nullopt;
  emit detailChanged();

  if (auto handler = m_model.onSelectionChanged) {
    if (it) { m_notify(handler->c_str(), {it->id.c_str()}); }
  }
}
