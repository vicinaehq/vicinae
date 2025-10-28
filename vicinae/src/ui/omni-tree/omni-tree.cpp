#include "omni-tree.hpp"

OmniTree::OmniTree(QWidget *parent) : QWidget(parent) {
  m_list->setMargins(5, 0, 5, 0);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(m_widget);
  layout->addWidget(m_list, 1);
  setLayout(layout);
  connect(m_list, &OmniList::itemActivated, this, &OmniTree::activateDelegate);
  connect(m_list, &OmniList::selectionChanged, this, &OmniTree::handleSelectionChanged);
}

bool OmniTree::selectUp() const { return m_list->selectUp(); }
bool OmniTree::selectDown() { return m_list->selectDown(); }
bool OmniTree::selectHome() const { return m_list->selectHome(); }
bool OmniTree::selectEnd() { return m_list->selectEnd(); }
void OmniTree::activateCurrentSelection() { m_list->activateCurrentSelection(); }
bool OmniTree::selectFirst() const { return m_list->selectFirst(); }

bool OmniTree::select(const QString &id) {
  // make sure we expand the section `id` is in if it is not, as it is technically
  // no part of the tree until expanded.
  for (const auto &row : m_model) {
    for (const auto &child : row->children()) {
      if (child->id() == id && !row->expanded()) {
        row->setExpandable(true);
        renderModel();
        break;
      }
    }
  }

  return m_list->setSelected(id);
}

VirtualTreeItemDelegate *OmniTree::value() const {
  return static_cast<const VirtualTreeItemRow *>(m_list->selected())->delegate();
}

void OmniTree::setHeader(HeaderWidget *widget) {
  if (auto item = layout->itemAt(0)) {
    if (auto previous = item->widget()) {
      layout->replaceWidget(previous, widget);
      previous->deleteLater();
    }
  }
}

void OmniTree::renderModel(OmniList::SelectionPolicy policy) {
  m_list->updateModel(
      [&]() {
        auto &section = m_list->addSection();
        size_t idx = 0;

        for (const auto &row : m_model) {
          auto item = std::make_shared<VirtualTreeItemRow>(row.get(), &m_header);

          if (idx % 2) item->setBackgroundColor(m_alternateBackgroundColor);

          section.addItem(item);
          ++idx;

          if (row->expanded()) {
            for (const auto &row2 : row->children()) {
              auto item = std::make_shared<VirtualTreeItemRow>(row2.get(), &m_header);

              if (idx % 2) item->setBackgroundColor(m_alternateBackgroundColor);
              item->setIndentLevel(1);
              section.addItem(item);
              ++idx;
            }
          }
        }
      },
      policy);
}

void OmniTree::setColumns(const std::vector<QString> &columns) {
  m_header.setColumns(columns);
  setHeader(new HeaderWidget(&m_header));
}

void OmniTree::setColumnSizePolicy(int idx, HeaderInfo::ColumnSizePolicy policy) {
  m_header.setColumnSizePolicy(idx, policy);
  setHeader(new HeaderWidget(&m_header));
}

void OmniTree::setColumnWidth(int index, int width) {
  m_header.setColumnWidth(index, width);
  setHeader(new HeaderWidget(&m_header));
}

void OmniTree::setAlternateBackgroundColor(const std::optional<ColorLike> &color) {
  m_alternateBackgroundColor = color;
}

void OmniTree::activateDelegate(const OmniList::AbstractVirtualItem &item) {
  auto &row = static_cast<const VirtualTreeItemRow &>(item);

  row.delegate()->setExpandable(!row.delegate()->expanded());
  renderModel();
}

void OmniTree::handleSelectionChanged(const OmniList::AbstractVirtualItem *next,
                                      const OmniList::AbstractVirtualItem *previous) {
  VirtualTreeItemDelegate *nextDelegate = nullptr;
  VirtualTreeItemDelegate *previousDelegate = nullptr;

  if (next) nextDelegate = static_cast<const VirtualTreeItemRow *>(next)->delegate();
  if (previous) previousDelegate = static_cast<const VirtualTreeItemRow *>(previous)->delegate();

  emit selectionUpdated(nextDelegate, previousDelegate);
}

auto OmniTree::model() const { return m_model; }

void OmniTree::refresh() { m_list->refresh(); }

VirtualTreeItemDelegate *OmniTree::itemAt(const QString &id) {
  auto item = m_list->itemAt(id);

  if (!item) return nullptr;

  return static_cast<const VirtualTreeItemRow *>(item)->delegate();
}

void OmniTree::addRows(std::vector<std::shared_ptr<VirtualTreeItemDelegate>> rows) {
  m_model = rows;
  renderModel(OmniList::SelectionPolicy::SelectNone);
}
