#pragma once
#include "action-panel/action-panel.hpp"
#include "common.hpp"
#include "extend/action-model.hpp"
#include "omni-icon.hpp"
#include "service-registry.hpp"
#include <algorithm>
#include <libqalculate/Calculator.h>
#include <qevent.h>
#include <qnamespace.h>
#include <ranges>
#include "ui/action-pannel/action.hpp"
#include "ui/horizontal-loading-bar.hpp"
#include "ui/keyboard.hpp"
#include "ui/omni-grid.hpp"
#include "ui/omni-list.hpp"
#include "ui/split-detail.hpp"
#include "ui/status_bar.hpp"
#include "ui/toast.hpp"
#include "ui/top_bar.hpp"
#include <memory>
#include <qboxlayout.h>
#include <qlogging.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class BaseView : public QWidget {
  Q_OBJECT

  bool m_initialized = false;

public:
  void createInitialize() {
    if (m_initialized) return;

    initialize();
    m_initialized = true;
  }
  bool isInitialized() { return m_initialized; }

  /**
   * Called before the view is first shown on screen.
   * You can use this hook to setup UI.
   */
  virtual void initialize() {}

  void activate() { onActivate(); }
  void deactivate() { onDeactivate(); }

  /**
   * Called when the view becomes visible. This is called the first time the view is shown
   * (right after `initialize`) but also after a view that was pushed on top of it was poped.
   */
  virtual void onActivate() {}

  /**
   * Called when the view becomes hidden. This is called before the view is poped or when
   * another view is pushed on top of it.
   */
  virtual void onDeactivate() {}

  /**
   * Called on the view when a toast needs to be displayed. It's up to the specific view to decide
   * how the toast should be shown.
   */
  virtual void setToast(const QString &title, ToastPriority priority) {}

  /**
   * Clear the content of the search bar. If this is not applicable to the view
   * (for instance, a form view doesn't have a search field) this method should not be overriden.
   */
  virtual void clearSearchBar() { qWarning() << "clearSearchBar() is not implemented for this view"; }

  /**
   * The current search text for this view. If not applicable, do not implement.
   */
  virtual QString searchText() const { return {}; }

  /**
   * Set the search text for the current view, if applicable
   */
  virtual void setSearchText(const QString &value) const {}

  /**
   * Set the navigation title, if applicable.
   */
  virtual void setNavigationTitle(const QString &title) {}

  /**
   * Set the navigation icon, if applicable
   */
  virtual void setNavigationIcon(const OmniIconUrl &icon) {}

  /**
   * The dynamic arguments for this view. Used by some actions.
   */
  virtual std::vector<QString> argumentValues() const { return {}; }

  BaseView(QWidget *parent = nullptr) : QWidget(parent) {}

signals:
  void requestLaunchCommand(const std::shared_ptr<AbstractCmd> &command, const LaunchCommandOptions &opts,
                            const LaunchProps &props) const;
  void requestViewPush(View *view, const PushViewOptions &options) const;
};

class SimpleView : public BaseView {
  KeyboardShortcutModel DEFAULT_ACTION_PANEL_SHORTCUT = {.key = "B", .modifiers = {"ctrl"}};

protected:
  QVBoxLayout *m_layout = new QVBoxLayout(this);
  TopBar *m_topBar = new TopBar(this);
  StatusBar *m_statusBar = new StatusBar(this);
  HorizontalLoadingBar *m_loadingBar = new HorizontalLoadingBar(this);
  ActionPanelV2Widget *m_actionPannelV2 = new ActionPanelV2Widget(this);

  KeyboardShortcutModel defaultActionPanelShortcut() { return DEFAULT_ACTION_PANEL_SHORTCUT; }

  bool eventFilter(QObject *obj, QEvent *event) override {
    if (event->type() == QEvent::KeyPress) {
      auto keyEvent = static_cast<QKeyEvent *>(event);
      bool isEsc = keyEvent->key() == Qt::Key_Escape;

      if (obj == m_topBar->input) { return inputFilter(keyEvent); }
    }

    return false;
  }

  virtual bool inputFilter(QKeyEvent *event) { return false; }

  bool event(QEvent *event) override {
    if (event->type() == QEvent::KeyPress) {
      auto keyEvent = static_cast<QKeyEvent *>(event);
      auto key = keyEvent->key();

      bool isEsc = keyEvent->key() == Qt::Key_Escape;

      if (isEsc) {
        escapePressed();
        return true;
      }

      if (KeyboardShortcut(m_statusBar->actionButtonShortcut()) == keyEvent) {
        // m_actionPannel->showActions();
        m_actionPannelV2->show();

        return true;
      }

      auto actions = m_actionPannelV2->actions();
      auto bound = std::ranges::find_if(actions, [&](const AbstractAction *action) {
        return action->shortcut && KeyboardShortcut(*action->shortcut) == keyEvent;
      });

      if (bound != actions.end()) {
        executeAction(*bound);
        return true;
      }
    }

    return QWidget::event(event);
  }

  void actionPannelOpened() {
    m_statusBar->setActionButtonHighlight(true);
    onActionPanelOpened();
  }

  virtual void escapePressed() {
    qDebug() << "escape pressed";
    ServiceRegistry::instance()->UI()->popView();
  }

  /**
   * Called when backspace is pressed on an empty search input
   */
  virtual void backspacePressed() { ServiceRegistry::instance()->UI()->popView(); }

  void actionPannelClosed() {
    m_statusBar->setActionButtonHighlight(false);
    onActionPanelClosed();
  }

  void executeAction(AbstractAction *action) {
    action->execute();
    onActionExecuted(action);
    if (action->isSubmenu()) {
      if (auto panel = action->createSubmenu()) {
        m_actionPannelV2->pushView(panel);
        m_actionPannelV2->show();
        return;
      }
    }
    m_actionPannelV2->close();
  }

  void actionButtonClicked() { m_actionPannelV2->show(); }

  void currentActionButtonClicked() {
    if (auto action = m_actionPannelV2->primaryAction()) { executeAction(action); }
  }

  virtual QWidget *centerWidget() const {
    qCritical() << "default centerWidget()";
    return new QWidget;
  }

  std::vector<QString> argumentValues() const override {
    return m_topBar->m_completer->collect() | std::views::transform([](const auto &p) { return p.second; }) |
           std::ranges::to<std::vector>();
  }

  void clearSearchBar() override { m_topBar->input->clear(); }

  void setToast(const QString &title, ToastPriority priority) override {
    m_statusBar->setToast(title, priority);
  }

  virtual void onSearchChanged(const QString &text) {}
  virtual void onActionPanelClosed() {}
  virtual void onActionPanelOpened() {}
  virtual void onActionExecuted(AbstractAction *action) {}

  void onActivate() override {
    if (!m_topBar->input->text().isEmpty()) { m_topBar->input->selectAll(); }

    m_topBar->input->setFocus();
  }

  void activatePrimaryAction() {
    if (auto action = m_actionPannelV2->primaryAction()) {
      executeAction(action);
    } else {
      m_actionPannelV2->show();
    }
  }

  void setupUI(QWidget *centerWidget) {
    m_topBar->setFixedHeight(Omnicast::TOP_BAR_HEIGHT);
    m_statusBar->setFixedHeight(Omnicast::STATUS_BAR_HEIGHT);

    m_loadingBar->setFixedHeight(1);
    m_loadingBar->setBarWidth(100);
    m_topBar->input->installEventFilter(this);

    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);
    m_layout->addWidget(m_topBar);
    m_layout->addWidget(m_loadingBar);
    m_layout->addWidget(centerWidget, 1);
    m_layout->addWidget(m_statusBar);
    setLayout(m_layout);

    connect(m_topBar->input, &SearchBar::textChanged, this, &SimpleView::onSearchChanged);
    connect(m_topBar->input, &SearchBar::pop, this, &SimpleView::backspacePressed);
    // connect(m_actionPannel, &ActionPannelWidget::opened, this, &SimpleView::actionPannelOpened);
    // connect(m_actionPannel, &ActionPannelWidget::closed, this, &SimpleView::actionPannelClosed);
    // connect(m_actionPannel, &ActionPannelWidget::actionExecuted, this, &SimpleView::executeAction);
    connect(m_actionPannelV2, &ActionPanelV2Widget::actionActivated, this, &SimpleView::executeAction);
    connect(m_statusBar, &StatusBar::actionButtonClicked, this, &SimpleView::actionButtonClicked);
  }

public:
  QString searchText() const override { return m_topBar->input->text(); }

  void setSearchText(const QString &value) const override { m_topBar->input->setText(value); }

  void setSearchText(const QString &text) { m_topBar->input->setText(text); }
  QString searchPlaceholderText() const { return m_topBar->input->placeholderText(); }
  void setSearchPlaceholderText(const QString &value) const {
    return m_topBar->input->setPlaceholderText(value);
  }

  QString navigationTitle() const { return m_statusBar->navigationTitle(); }
  void setNavigationTitle(const QString &title) override { m_statusBar->setNavigationTitle(title); }
  void setNavigationIcon(const OmniIconUrl &url) override { m_statusBar->setNavigationIcon(url); }

  void setLoading(bool value) { m_loadingBar->setStarted(value); }

  void initialize() override {}

public:
  SimpleView(QWidget *parent = nullptr) : BaseView(parent) { m_topBar->showBackButton(); }
};

class ListView : public SimpleView {
  SplitDetailWidget *m_split = new SplitDetailWidget(this);

  void keyPressEvent(QKeyEvent *event) override {
    switch (event->key()) {
    case Qt::Key_Up:
      m_list->selectUp();
      break;
    case Qt::Key_Down:
      m_list->selectDown();
      break;
    case Qt::Key_Return:
      m_list->activateCurrentSelection();
      break;
    }
  }

public:
  struct Actionnable {
    virtual QList<AbstractAction *> generateActions() const { return {}; };
    virtual QWidget *generateDetail() const { return nullptr; }
    virtual std::unique_ptr<CompleterData> createCompleter() const { return nullptr; }
    virtual QString navigationTitle() const { return {}; }

    virtual ActionPanelView *actionPanel() const {
      auto panel = new ActionPanelStaticListView;

      for (const auto &action : generateActions()) {
        panel->addAction(action);
      }

      return panel;
    }

    /**
     * Current action title to show in the status bar. Only shown if no primary action has been set.
     */
    virtual QString actionPanelTitle() const { return "Actions"; }

    // Whether to show the "Actions <action_shortcut>" next to the current action title
    bool showActionButton() const { return true; }
  };

protected:
  OmniList *m_list = new OmniList();

  virtual void modelChanged() {}

  virtual void itemSelected(const OmniList::AbstractVirtualItem *item) {}

  virtual void selectionChanged(const OmniList::AbstractVirtualItem *next,
                                const OmniList::AbstractVirtualItem *previous) {
    if (!next) {
      m_split->setDetailVisibility(false);
      m_topBar->destroyCompleter();
      // setNavigationTitle(QString("%1").arg(m_baseNavigationTitle));

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
        m_topBar->activateCompleter(*completer);
      } else {
        m_topBar->destroyCompleter();
      }

      // TODO: only expect suffix and automatically use command name from prefix
      if (auto navigation = nextItem->navigationTitle(); !navigation.isEmpty()) {
        // setNavigationTitle(QString("%1 - %2").arg(m_baseNavigationTitle).arg(navigation));
      }

      if (auto panel = nextItem->actionPanel()) {
        m_actionPannelV2->setView(panel);
      } else {
        m_actionPannelV2->hide();
        m_actionPannelV2->popToRoot();
      }

      m_statusBar->setActionButton(nextItem->actionPanelTitle(), KeyboardShortcutModel{.key = "return"});

      auto actions = m_actionPannelV2->actions();
      auto primaryAction = m_actionPannelV2->primaryAction();

      m_statusBar->setActionButtonVisibility(!primaryAction || actions.size() > 1);
      m_statusBar->setCurrentActionButtonVisibility(primaryAction);

      if (auto action = m_actionPannelV2->primaryAction()) {
        m_statusBar->setCurrentAction(action->title(),
                                      action->shortcut.value_or(KeyboardShortcutModel{.key = "return"}));
        m_statusBar->setActionButton("Actions", defaultActionPanelShortcut());
      } else {
        m_statusBar->setActionButton(nextItem->actionPanelTitle(), KeyboardShortcutModel{.key = "return"});
      }

    } else {
      m_split->setDetailVisibility(false);
      m_topBar->destroyCompleter();
      m_actionPannelV2->popToRoot();
      m_actionPannelV2->close();
    }

    itemSelected(next);
  }

  virtual void itemActivated(const OmniList::AbstractVirtualItem &item) { activatePrimaryAction(); }

  QWidget *detail() const { return m_split->detailWidget(); }

  void setDetail(QWidget *widget) {
    m_split->setDetailWidget(widget);
    m_split->setDetailVisibility(true);
  }

public:
  ListView(QWidget *parent = nullptr) : SimpleView(parent) {
    m_split->setMainWidget(m_list);
    setupUI(m_split);
    connect(m_list, &OmniList::modelChanged, this, &ListView::modelChanged);
    connect(m_list, &OmniList::selectionChanged, this, &ListView::selectionChanged);
    connect(m_list, &OmniList::itemActivated, this, &ListView::itemActivated);
  }
};

class GridView : public SimpleView {
  bool inputFilter(QKeyEvent *event) override {
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

public:
  struct Actionnable {
    virtual QList<AbstractAction *> generateActions() const { return {}; };
    virtual QString navigationTitle() const { return {}; }
    virtual ActionPanelView *actionPanel() const {
      auto panel = new ActionPanelStaticListView;

      for (const auto &action : generateActions()) {
        panel->addAction(action);
      }

      return panel;
    }

    /**
     * Current action title to show in the status bar. Only shown if no primary action has been set.
     */
    virtual QString actionPanelTitle() const { return "Actions"; }
  };

protected:
  OmniGrid *m_grid = new OmniGrid();

  virtual void selectionChanged(const OmniList::AbstractVirtualItem *next,
                                const OmniList::AbstractVirtualItem *previous) {

    if (!next) {
      m_topBar->destroyCompleter();
      // setNavigationTitle(QString("%1").arg(m_baseNavigationTitle));

      return;
    }

    if (auto nextItem = dynamic_cast<const Actionnable *>(next)) {
      // TODO: only expect suffix and automatically use command name from prefix
      if (auto navigation = nextItem->navigationTitle(); !navigation.isEmpty()) {
        // setNavigationTitle(QString("%1 - %2").arg(m_baseNavigationTitle).arg(navigation));
      }

      if (auto panel = nextItem->actionPanel()) {
        m_actionPannelV2->setView(panel);
      } else {
        m_actionPannelV2->hide();
        m_actionPannelV2->popToRoot();
      }

      m_statusBar->setActionButton(nextItem->actionPanelTitle(), KeyboardShortcutModel{.key = "return"});

      auto actions = m_actionPannelV2->actions();
      auto primaryAction = m_actionPannelV2->primaryAction();

      m_statusBar->setActionButtonVisibility(!primaryAction || actions.size() > 1);
      m_statusBar->setCurrentActionButtonVisibility(primaryAction);

      if (auto action = m_actionPannelV2->primaryAction()) {
        m_statusBar->setCurrentAction(action->title(),
                                      action->shortcut.value_or(KeyboardShortcutModel{.key = "return"}));
        m_statusBar->setActionButton("Actions", defaultActionPanelShortcut());
      } else {
        m_statusBar->setActionButton(nextItem->actionPanelTitle(), KeyboardShortcutModel{.key = "return"});
      }

    } else {
      m_topBar->destroyCompleter();
      m_actionPannelV2->popToRoot();
      m_actionPannelV2->close();
    }
  }

  virtual void itemActivated(const OmniList::AbstractVirtualItem &item) { activatePrimaryAction(); }

public:
  GridView(QWidget *parent = nullptr) : SimpleView(parent) {
    setupUI(m_grid);
    connect(m_grid, &OmniList::selectionChanged, this, &GridView::selectionChanged);
    connect(m_grid, &OmniList::itemActivated, this, &GridView::itemActivated);
  }
};

class FormView : public SimpleView {
public:
  FormView(QWidget *parent = nullptr) : SimpleView(parent) { m_topBar->input->hide(); }
};
