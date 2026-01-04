#include "extension-grid.hpp"
#include "ui/omni-grid/grid-item-content-widget.hpp"

using namespace vicinae::ui;

ExtensionGridList::ExtensionGridList() {
  auto layout = new QVBoxLayout;

  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(m_list);
  setLayout(layout);
  m_list->setMargins(QMargins(20, 10, 20, 10));
  m_list->setModel(m_model);

  connect(m_list, &VListWidget::itemSelected, this, &ExtensionGridList::handleSelectionChanged);
  connect(m_list, &VListWidget::itemActivated, this, &ExtensionGridList::handleItemActivated);
}

void ExtensionGridList::setColumns(int cols) { m_model->setColumns(cols); }

void ExtensionGridList::setAspectRatio(double ratio) { m_model->setAspectRatio(ratio); }
void ExtensionGridList::setFit(ObjectFit fit) { m_model->setFit(fit); }

void ExtensionGridList::setInset(GridItemContentWidget::Inset inset) { m_model->setInset(inset); }

bool ExtensionGridList::selectUp() { return m_list->selectUp(); }
bool ExtensionGridList::selectDown() { return m_list->selectDown(); }
bool ExtensionGridList::selectLeft() { return m_list->selectLeft(); }
bool ExtensionGridList::selectRight() { return m_list->selectRight(); }
void ExtensionGridList::selectNext() { m_list->selectNext(); }
void ExtensionGridList::activateCurrentSelection() const { m_list->activateCurrentSelection(); }

bool ExtensionGridList::empty() const { return m_model->isEmpty(); }

void ExtensionGridList::setModel(const std::vector<GridChild> &model, OmniList::SelectionPolicy selection) {
  m_model->setData(model);
  m_model->reload();
  m_list->refreshAll();
}

void ExtensionGridList::setFilter(const QString &query) {
  m_model->setFilter(query);
  m_list->selectFirst();
}

GridItemViewModel const *ExtensionGridList::selected() const {
  auto idx = m_list->currentSelection();

  if (!idx) {
    emit selectionChanged(nullptr);
    return nullptr;
  }

  if (auto item = m_model->fromIndex(*idx)) { return *item; }
  return nullptr;
}

void ExtensionGridList::handleSelectionChanged(const std::optional<vicinae::ui::VListModel::Index> idx) {
  if (!idx) {
    emit selectionChanged(nullptr);
    return;
  }

  if (auto item = m_model->fromIndex(*idx)) { emit selectionChanged(*item); }
}

void ExtensionGridList::handleItemActivated(vicinae::ui::VListModel::Index idx) {
  if (auto item = m_model->fromIndex(idx)) { emit itemActivated(**item); }
}
