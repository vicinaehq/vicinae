#include "grid-view.hpp"
#include "ui/empty-view/empty-view.hpp"
#include "ui/omni-grid/omni-grid.hpp"
#include <qevent.h>
#include <qstackedwidget.h>
#include "navigation-controller.hpp"

bool GridView::inputFilter(QKeyEvent *event) {
  switch (event->key()) {
  case Qt::Key_Up:
    return m_grid->selectUp();
  case Qt::Key_Down:
    return m_grid->selectDown();
  case Qt::Key_Left:
    return m_grid->selectLeft();
  case Qt::Key_Right:
    return m_grid->selectRight();
  }

  return false;
}

void GridView::onActivate() {
  if (auto selection = m_grid->selected()) {
    if (auto nextItem = dynamic_cast<const Actionnable *>(selection)) { applyActionnable(nextItem); }
  }
}

void GridView::applyActionnable(const Actionnable *actionnable) {
  if (auto navigation = actionnable->navigationTitle(); !navigation.isEmpty()) {
    setNavigationTitle(QString("%1 - %2").arg(rootNavigationTitle()).arg(navigation));
  }
}

void GridView::selectionChanged(const OmniList::AbstractVirtualItem *next,
                                const OmniList::AbstractVirtualItem *previous) {
  if (!next) {
    context()->navigation->clearActions(this);
    setNavigationTitle(rootNavigationTitle());
    return;
  }

  if (auto nextItem = dynamic_cast<const Actionnable *>(next)) {
    setActions(nextItem->newActionPanel(context()));
    applyActionnable(nextItem);
  } else {
    context()->navigation->clearActions(this);
    destroyCompleter();
  }
}

void GridView::itemActivated(const OmniList::AbstractVirtualItem &item) { executePrimaryAction(); }

GridView::GridView(QWidget *parent) : SimpleView(parent) {
  m_grid = new OmniGrid();
  m_content = new QStackedWidget(this);
  m_emptyView = new EmptyViewWidget(this);

  m_content->addWidget(m_grid);
  m_content->addWidget(m_emptyView);
  m_content->setCurrentWidget(m_grid);
  m_emptyView->setTitle("No results");
  m_emptyView->setDescription("No results matching your search. You can try to refine your search.");
  m_emptyView->setIcon(BuiltinOmniIconUrl("magnifying-glass"));

  setupUI(m_content);
  connect(m_grid, &OmniList::selectionChanged, this, &GridView::selectionChanged);
  connect(m_grid, &OmniList::itemActivated, this, &GridView::itemActivated);
  connect(m_grid, &OmniList::virtualHeightChanged, this, [this](int height) {
    if (m_grid->items().empty() && !searchText().isEmpty()) {
      m_content->setCurrentWidget(m_emptyView);
      return;
    }

    m_content->setCurrentWidget(m_grid);
  });
}
