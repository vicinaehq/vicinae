#include "extension-list.hpp"
#include "layout.hpp"
#include "ui/vlist/vlist.hpp"

using namespace vicinae::ui;

ExtensionList::ExtensionList() {
  VStack().add(m_list).imbue(this);
  m_list->setModel(m_model);
  connect(m_list, &VListWidget::itemSelected, this, &ExtensionList::handleSelectionChanged);
  connect(m_list, &VListWidget::itemActivated, this, &ExtensionList::handleItemActivated);
}

bool ExtensionList::selectUp() { return m_list->selectUp(); }
bool ExtensionList::selectDown() { return m_list->selectDown(); }
void ExtensionList::selectNext() { m_list->selectNext(); }
void ExtensionList::activateCurrentSelection() const { m_list->activateCurrentSelection(); }

bool ExtensionList::empty() const { return m_model->isEmpty(); }

void ExtensionList::setModel(const std::vector<ListChild> &model, OmniList::SelectionPolicy selection) {
  m_model->setData(model);
  m_model->reload();
  m_list->refreshAll();
}

void ExtensionList::setFilter(const QString &query) {
  m_model->setFilter(query);
  m_list->selectFirst();
}

ListItemViewModel const *ExtensionList::selected() const {
  auto idx = m_list->currentSelection();

  if (!idx) {
    emit selectionChanged(nullptr);
    return nullptr;
  }

  if (auto item = m_model->fromIndex(*idx)) { return *item; }
  return nullptr;
}

void ExtensionList::handleSelectionChanged(const std::optional<vicinae::ui::VListModel::Index> idx) {
  if (!idx) {
    emit selectionChanged(nullptr);
    return;
  }

  if (auto item = m_model->fromIndex(*idx)) { emit selectionChanged(*item); }
}

void ExtensionList::handleItemActivated(vicinae::ui::VListModel::Index idx) {
  if (auto item = m_model->fromIndex(idx)) { emit itemActivated(**item); }
}
