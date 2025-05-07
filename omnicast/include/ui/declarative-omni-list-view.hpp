#pragma once
#include "ui/action-pannel/action-item.hpp"
#include "ui/omni-list.hpp"
#include "ui/split-detail.hpp"
#include "view.hpp"
#include <qboxlayout.h>
#include <qevent.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qwidget.h>

class DeclarativeOmniListView : public View {
protected:
  OmniList *list;
  QString lastSelectedId;
  QString m_baseNavigationTitle;
  SplitDetailWidget *m_split = new SplitDetailWidget(this);

  using ItemList = std::vector<std::unique_ptr<OmniList::AbstractVirtualItem>>;

public:
  class IActionnable {
  public:
    virtual QList<AbstractAction *> generateActions() const { return {}; };
    virtual QWidget *generateDetail() const { return nullptr; }
    virtual std::unique_ptr<CompleterData> createCompleter() const { return nullptr; }
    virtual std::vector<ActionItem> generateActionPannel() const { return {}; };
    virtual QString navigationTitle() const { return {}; }
  };

protected:
  virtual void selectionChanged(const OmniList::AbstractVirtualItem *next,
                                const OmniList::AbstractVirtualItem *previous) {
    if (!next) {
      m_split->setDetailVisibility(false);
      app.topBar->destroyQuicklinkCompleter();
      setSignalActions({});
      app.statusBar->setNavigationTitle(QString("%1").arg(m_baseNavigationTitle));

      return;
    }

    if (auto nextItem = dynamic_cast<const IActionnable *>(next)) {
      if (auto detail = nextItem->generateDetail()) {
        if (auto current = m_split->detailWidget()) { current->deleteLater(); }
        m_split->setDetailWidget(detail);
        m_split->setDetailVisibility(true);
      } else {
        m_split->setDetailVisibility(false);
      }

      if (auto completer = nextItem->createCompleter(); completer && completer->arguments.size() > 0) {
        app.topBar->activateCompleter(*completer);
      } else {
        app.topBar->destroyCompleter();
      }

      // TODO: only expect suffix and automatically use command name from prefix
      if (auto navigation = nextItem->navigationTitle(); !navigation.isEmpty()) {
        app.statusBar->setNavigationTitle(QString("%1 - %2").arg(m_baseNavigationTitle).arg(navigation));
      }

      auto pannel = nextItem->generateActionPannel();

      if (!pannel.empty()) {
        int idx = 0;

        for (const auto &item : pannel) {
          if (auto action = std::get_if<std::shared_ptr<AbstractAction>>(&item)) {
            if (idx == 0) (*action)->setShortcut({.key = "return", .modifiers = {}});
            if (idx == 1) (*action)->setShortcut({.key = "return", .modifiers = {"shift"}});
            ++idx;
          }
        }

        setActionPannel(pannel);

        // if (auto first = app.actionPannel->actionnable(0)) { first->setShortcut({.key = "return"}); }
      } else {

        auto actions = nextItem->generateActions();

        if (!actions.isEmpty()) { actions.at(0)->setShortcut({.key = "return"}); }
        if (actions.size() > 1) { actions.at(1)->setShortcut({.key = "return", .modifiers = {"shift"}}); }

        setSignalActions(actions);
      }
    } else {
      m_split->setDetailVisibility(false);
      app.topBar->destroyCompleter();
      setSignalActions({});
    }

    lastSelectedId = next->id();
  }

  virtual void itemActivated(const OmniList::AbstractVirtualItem &item) {
    qDebug() << "activated";
    emit activatePrimaryAction();
  }

  void recreateCurrentActions() {
    if (auto item = list->selected()) {
      if (auto nextItem = dynamic_cast<const IActionnable *>(item)) {
        setSignalActions(nextItem->generateActions());
      }
    }
  }

  bool inputFilter(QKeyEvent *event) override {
    switch (event->key()) {
    case Qt::Key_Left:
    case Qt::Key_Right:
    case Qt::Key_Up:
    case Qt::Key_Down:
    case Qt::Key_Return:
      QApplication::sendEvent(list, event);
      return true;
    }

    return View::inputFilter(event);
  }

  QString query;

  virtual ItemList generateList(const QString &s) { return {}; }

  virtual void render(const QString &s) {}
  virtual bool doesUseNewModel() const { return false; }

  void reload(OmniList::SelectionPolicy policy = OmniList::KeepSelection) {
    qDebug() << "reload list";

    if (doesUseNewModel()) {
      list->beginResetModel();
      render(query);
      list->endResetModel(policy);
    } else {
      auto items = generateList(query);
      list->updateFromList(items, policy);
    }
  }

  void onActionActivated(const AbstractAction *action) override {
    if (isVisible()) {
      qDebug() << "action activated!";
      reload();
    } else {
      qDebug() << "no reload after action, as we are no longer visible!";
    }
  }

  void onRestore() override { /*reload(OmniList::KeepSelection);*/ }

  void onMount() override { setBaseNavigationTitle(navigationTitle()); }

  void setBaseNavigationTitle(const QString &value) { m_baseNavigationTitle = value; }

  void onSearchChanged(const QString &s) override {
    query = s;

    if (doesUseNewModel()) {
      list->beginResetModel();
      render(s);
      list->endResetModel(OmniList::SelectFirst);
    } else {
      auto items = generateList(s);
      list->updateFromList(items, OmniList::SelectFirst);
    }
  }

  void setDetailWidget(QWidget *widget) { m_split->setDetailWidget(widget); }

public:
  DeclarativeOmniListView(AppWindow &app) : View(app), list(new OmniList) {
    m_split->setMainWidget(list);
    list->setObjectName("DECL_LIST");

    connect(list, &OmniList::selectionChanged, this, &DeclarativeOmniListView::selectionChanged);
    connect(list, &OmniList::itemActivated, this, &DeclarativeOmniListView::itemActivated);

    auto layout = new QVBoxLayout;

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_split);
    setLayout(layout);
  }

  ~DeclarativeOmniListView() { qCritical() << "~DeclarativeOmniListView"; }
};
