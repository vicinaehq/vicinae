#include "list-view.hpp"
#include "navigation-controller.hpp"
#include "ui/omni-list/omni-list.hpp"
#include "ui/views/base-view.hpp"
#include "ui/empty-view/empty-view.hpp"
#include "ui/split-detail/split-detail.hpp"
#include "services/keybinding/keybinding-service.hpp"
#include "services/config/config-service.hpp"
#include "service-registry.hpp"
#include <qlogging.h>
#include <qstackedwidget.h>

bool ListView::inputFilter(QKeyEvent *event) {
  auto config = ServiceRegistry::instance()->config();
  const QString &keybinding = config->value().keybinding;

  if (event->modifiers() == Qt::ControlModifier) {
    if (KeyBindingService::isDownKey(event, keybinding)) { return m_list->selectDown(); }
    if (KeyBindingService::isUpKey(event, keybinding)) { return m_list->selectUp(); }
    if (KeyBindingService::isLeftKey(event, keybinding)) {
      context()->navigation->popCurrentView();
      return true;
    }
    if (KeyBindingService::isRightKey(event, keybinding)) {
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
    case Qt::Key_Tab: {
      if (!context()->navigation->hasCompleter()) {
        m_list->selectNext();
        return true;
      }
      break;
    }
    case Qt::Key_Home:
      return m_list->selectHome();
      break;
    case Qt::Key_End:
      return m_list->selectEnd();
      break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
      m_list->activateCurrentSelection();
      return true;
    }
  }

  return SimpleView::inputFilter(event);
}

void ListView::itemSelected(const OmniList::AbstractVirtualItem *item) {}

void ListView::forceReselection() { selectionChanged(m_list->selected(), nullptr); }

void ListView::emptied() {}

void ListView::selectionChanged(const OmniList::AbstractVirtualItem *next,
                                const OmniList::AbstractVirtualItem *previous) {
  auto &nav = context()->navigation;

  if (!next) {
    m_split->setDetailVisibility(false);
    clearActions();
    emptied();
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
      nav->createCompletion(completer->arguments, completer->iconUrl);
    } else {
      nav->destroyCurrentCompletion();
    }

    // TODO: only expect suffix and automatically use command name from prefix
    if (auto navigation = nextItem->navigationTitle(); !navigation.isEmpty()) {
      // setNavigationTitle(QString("%1 - %2").arg(m_baseNavigationTitle).arg(navigation));
      //
    }

    auto panel = nextItem->newActionPanel(context());

    panel->setShortcutPreset(ActionPanelState::ShortcutPreset::List);
    setActions(std::move(panel));
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
    if (m_list->items().empty() && (!searchText().isEmpty() || !isLoading())) {
      // ui->destroyCompleter();
      m_content->setCurrentWidget(m_emptyView);
      return;
    }

    m_content->setCurrentWidget(m_split);
  });
}
