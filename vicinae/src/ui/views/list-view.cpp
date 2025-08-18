#include "list-view.hpp"
#include "ui/omni-list/omni-list.hpp"
#include "ui/views/base-view.hpp"
#include "ui/empty-view/empty-view.hpp"
#include "ui/split-detail/split-detail.hpp"
#include <qlogging.h>
#include <qstackedwidget.h>

bool ListView::inputFilter(QKeyEvent *event) {
  if (event->modifiers() == Qt::ControlModifier) {
    switch (event->key()) {
    case Qt::Key_J:
      return m_list->selectDown();
    case Qt::Key_K:
      return m_list->selectUp();
    case Qt::Key_H:
      context()->navigation->popCurrentView();
      return true;
    case Qt::Key_L:
      m_list->activateCurrentSelection();
      return true;
    }
  }

  if (event->modifiers().toInt() == 0) {
    switch (event->key()) {
    case Qt::Key_Up:
      return m_list->selectUp();
      break;
    case Qt::Key_Down:
      return m_list->selectDown();
      break;
    case Qt::Key_Return:
      m_list->activateCurrentSelection();
      return true;
    }
  }

  return SimpleView::inputFilter(event);
}

void ListView::itemSelected(const OmniList::AbstractVirtualItem *item) {}

void ListView::selectionChanged(const OmniList::AbstractVirtualItem *next,
                                const OmniList::AbstractVirtualItem *previous) {
  if (!next) {
    m_split->setDetailVisibility(false);
    return;
  }

  if (auto nextItem = dynamic_cast<const Actionnable *>(next)) {
    if (auto detail = nextItem->generateDetail()) {
      if (auto current = m_split->detailWidget()) { current->deleteLater(); }
      m_split->setDetailWidget(detail);
      m_split->setDetailVisibility(true);
    } else {
      m_split->setDetailVisibility(false);
    }

    if (auto completer = nextItem->createCompleter(); completer && completer->arguments.size() > 0) {
      context()->navigation->createCompletion(completer->arguments, completer->iconUrl);
    } else {
      context()->navigation->destroyCurrentCompletion();
    }

    // TODO: only expect suffix and automatically use command name from prefix
    if (auto navigation = nextItem->navigationTitle(); !navigation.isEmpty()) {
      // setNavigationTitle(QString("%1 - %2").arg(m_baseNavigationTitle).arg(navigation));
      //
    }

    context()->navigation->setActions(nextItem->newActionPanel(context()));

  } else {
    m_split->setDetailVisibility(false);
    context()->navigation->destroyCurrentCompletion();
  }

  itemSelected(next);
}

void ListView::itemActivated(const OmniList::AbstractVirtualItem &item) { executePrimaryAction(); }

QWidget *ListView::detail() const { return m_split->detailWidget(); }

void ListView::setDetail(QWidget *widget) {
  m_split->setDetailWidget(widget);
  m_split->setDetailVisibility(true);
}

void ListView::itemRightClicked(const OmniList::AbstractVirtualItem &item) {
  m_list->setSelected(item.id(), OmniList::ScrollBehaviour::ScrollRelative);
}

void ListView::setupUI(QWidget *center) {
  m_split = new SplitDetailWidget(this);
  m_content = new QStackedWidget(this);
  m_emptyView = new EmptyViewWidget(this);
  m_list = new OmniList();
  m_content->addWidget(m_split);
  m_content->addWidget(m_emptyView);
  m_content->setCurrentWidget(m_split);

  m_emptyView->setTitle("No results");
  m_emptyView->setDescription("No results matching your search. You can try to refine your search.");
  m_emptyView->setIcon(ImageURL::builtin("magnifying-glass"));

  m_split->setMainWidget(m_list);

  SimpleView::setupUI(m_content);
}

ListView::ListView(QWidget *parent) : SimpleView(parent) {
  setupUI(nullptr);
  connect(m_list, &OmniList::selectionChanged, this, &ListView::selectionChanged);
  connect(m_list, &OmniList::itemActivated, this, &ListView::itemActivated);
  connect(m_list, &OmniList::itemRightClicked, this, &ListView::itemRightClicked);
  connect(m_list, &OmniList::virtualHeightChanged, this, [this](int height) {
    if (m_list->items().empty() && !searchText().isEmpty()) {
      // ui->destroyCompleter();
      m_content->setCurrentWidget(m_emptyView);
      return;
    }

    m_content->setCurrentWidget(m_split);
  });
}
