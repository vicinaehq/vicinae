#pragma once
#include "action-panel/action-panel.hpp"
#include "argument.hpp"
#include "common.hpp"
#include "extend/action-model.hpp"
#include "omni-icon.hpp"
#include "service-registry.hpp"
#include <algorithm>
#include <libqalculate/Calculator.h>
#include <qevent.h>
#include <qnamespace.h>
#include "ui/action-pannel/action.hpp"
#include "ui/horizontal-loading-bar.hpp"
#include "ui/keyboard.hpp"
#include "ui/omni-grid.hpp"
#include "ui/omni-list.hpp"
#include "ui/split-detail.hpp"
#include "ui/toast.hpp"
#include "ui/top_bar.hpp"
#include "ui/ui-controller.hpp"
#include <memory>
#include <qboxlayout.h>
#include <qlogging.h>
#include <qobjectdefs.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class BaseView : public QWidget {
  Q_OBJECT

  bool m_initialized = false;
  std::unique_ptr<UIViewController> m_uiController;

public:
  void createInitialize() {
    if (m_initialized) return;

    initialize();
    m_initialized = true;
  }
  bool isInitialized() { return m_initialized; }

  /**
   * Whether to show the search bar for this view. Calling setSearchText or searchText() is still
   * valid but will always return the empty string.
   */
  virtual bool supportsSearch() const { return true; }

  virtual bool needsGlobalStatusBar() const { return true; }
  virtual bool needsGlobalTopBar() const { return true; }

  void setActionPanelWidget(ActionPanelV2Widget *widget) { m_uiController->setActionPanelWidget(widget); }

  /**
   * Called before the view is first shown on screen.
   * You can use this hook to setup UI.
   */
  virtual void initialize() {}

  void activate() { onActivate(); }
  void deactivate() { onDeactivate(); }

  virtual void argumentValuesChanged(const std::vector<std::pair<QString, QString>> &arguments) {}

  virtual void executeAction(AbstractAction *action) {}

  /**
   * Received when the global text search bar updates.
   */
  virtual void textChanged(const QString &text) {}

  void setSearchAccessory(QWidget *accessory) { m_uiController->setSearchAccessory(accessory); }

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
  virtual void setToast(const Toast *toast) {}

  virtual void clearToast() {}

  void activateCompleter(const ArgumentList &args, const OmniIconUrl &icon) {
    m_uiController->activateCompleter(args, icon);
  }

  void destroyCompleter() { m_uiController->destroyCompleter(); }

  virtual QWidget *searchBarAccessory() const { return nullptr; }

  QString searchPlaceholderText() const { return ""; }
  void setSearchPlaceholderText(const QString &value) const {
    m_uiController->setSearchPlaceholderText(value);
  }

  /**
   * Clear the content of the search bar. If this is not applicable to the view
   * (for instance, a form view doesn't have a search field) this method should not be overriden.
   */
  virtual void clearSearchBar() { qWarning() << "clearSearchBar() is not implemented for this view"; }

  /**
   * The current search text for this view. If not applicable, do not implement.
   */
  QString searchText() const { return m_uiController->searchText(); }

  /**
   * Set the search text for the current view, if applicable
   */
  void setSearchText(const QString &value) { m_uiController->setSearchText(value); }

  virtual ActionPanelV2Widget *actionPanel() const { return nullptr; }

  /**
   * Set the navigation title, if applicable.
   */
  virtual void setNavigationTitle(const QString &title) {}

  /**
   * Allows the view to filter input from the main search bar before the input itself
   * processes it.
   * For instance, this allows a list view to capture up and down events to move the position in the list.
   * Or, as an example that actually modifies the input behaviour, a grid list (with horizontal controls) can
   * repurpose the left and right keys to navigate the list, while they would normally move the text cursor.
   *
   * In typical QT event filter fashion, this function should return false if the key is left for the input
   * to handle, or true if it needs to be ignored.
   *
   */
  virtual bool inputFilter(QKeyEvent *event) { return false; }

  /**
   * Set the navigation icon, if applicable
   */
  virtual void setNavigationIcon(const OmniIconUrl &icon) {}

  void setNavigation(const QString &title, const OmniIconUrl &icon) {
    m_uiController->setNavigation(title, icon);
  }

  void setSearchVisiblity(bool value) { m_uiController->setSearchVisiblity(value); }

  void setLoading(bool value) { m_uiController->setLoading(value); }

  /**
   * The dynamic arguments for this view. Used by some actions.
   */
  virtual std::vector<QString> argumentValues() const { return {}; }

  BaseView(QWidget *parent = nullptr) : QWidget(parent) {
    auto ui = ServiceRegistry::instance()->UI();

    m_uiController = std::make_unique<UIViewController>(ui, this);
  }

signals:
  void requestLaunchCommand(const std::shared_ptr<AbstractCmd> &command, const LaunchCommandOptions &opts,
                            const LaunchProps &props) const;
  void requestViewPush(View *view, const PushViewOptions &options) const;
};

class SimpleView : public BaseView {
protected:
  QVBoxLayout *m_layout = new QVBoxLayout(this);
  ActionPanelV2Widget *m_actionPannelV2 = new ActionPanelV2Widget(this);

  void executeAction(AbstractAction *action) override {
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

  virtual QWidget *centerWidget() const {
    qCritical() << "default centerWidget()";
    return new QWidget;
  }

  virtual void onSearchChanged(const QString &text) {}
  virtual void onActionPanelClosed() {}
  virtual void onActionPanelOpened() {}
  virtual void onActionExecuted(AbstractAction *action) {}

  void onActivate() override {
    // m_topBar->input->setFocus();
  }

  void activatePrimaryAction() {
    auto ui = ServiceRegistry::instance()->UI();

    ui->executeDefaultAction();
  }

  void setupUI(QWidget *centerWidget) {
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);
    // m_layout->addWidget(m_topBar);
    m_layout->addWidget(centerWidget, 1);
    setLayout(m_layout);
  }

public:
  std::vector<QString> argumentValues() const override {
    return {};
    // return m_topBar->m_completer->collect() | std::views::transform([](const auto &p) { return p.second; })
    // | std::ranges::to<std::vector>();
  }

  void clearSearchBar() override { setSearchText(""); }

  void initialize() override {}

public:
  ActionPanelV2Widget *actionPanel() const override { return m_actionPannelV2; }

  SimpleView(QWidget *parent = nullptr) : BaseView(parent) {
    // m_topBar->showBackButton();
  }
};

class ListView : public SimpleView {
  SplitDetailWidget *m_split = new SplitDetailWidget(this);

  virtual bool inputFilter(QKeyEvent *event) override {
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

    return SimpleView::inputFilter(event);
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
      // m_topBar->destroyCompleter();
      //   setNavigationTitle(QString("%1").arg(m_baseNavigationTitle));

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
        activateCompleter(completer->arguments, completer->iconUrl);
      } else {
        destroyCompleter();
      }

      // TODO: only expect suffix and automatically use command name from prefix
      if (auto navigation = nextItem->navigationTitle(); !navigation.isEmpty()) {
        // setNavigationTitle(QString("%1 - %2").arg(m_baseNavigationTitle).arg(navigation));
        //
      }

      auto ui = ServiceRegistry::instance()->UI();

      if (auto panel = nextItem->actionPanel()) {
        m_actionPannelV2->setView(panel);
      } else {
        m_actionPannelV2->hide();
        m_actionPannelV2->popToRoot();
      }

    } else {
      m_split->setDetailVisibility(false);
      destroyCompleter();
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

  void itemRightClicked(const OmniList::AbstractVirtualItem &item) {
    m_list->setSelected(item.id(), OmniList::ScrollBehaviour::ScrollRelative);
    m_actionPannelV2->show();
  }

public:
  ListView(QWidget *parent = nullptr) : SimpleView(parent) {
    m_split->setMainWidget(m_list);
    setupUI(m_split);
    connect(m_list, &OmniList::modelChanged, this, &ListView::modelChanged);
    connect(m_list, &OmniList::selectionChanged, this, &ListView::selectionChanged);
    connect(m_list, &OmniList::itemActivated, this, &ListView::itemActivated);
    connect(m_list, &OmniList::itemRightClicked, this, &ListView::itemRightClicked);
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
    case Qt::Key_Return:
      m_grid->activateCurrentSelection();
      return true;
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
    auto ui = ServiceRegistry::instance()->UI();

    if (!next) {
      // m_topBar->destroyCompleter();
      //  setNavigationTitle(QString("%1").arg(m_baseNavigationTitle));

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
        // m_actionPannelV2->hide();
        // m_actionPannelV2->popToRoot();
      }

      /*
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
      */

    } else {
      ui->clearActionPanel();
      // m_topBar->destroyCompleter();
    }
  }

  virtual void itemActivated(const OmniList::AbstractVirtualItem &item) {
    auto ui = ServiceRegistry::instance()->UI();

    qDebug() << "actionnable";

    ui->executeDefaultAction();
  }

public:
  GridView(QWidget *parent = nullptr) : SimpleView(parent) {
    setupUI(m_grid);
    connect(m_grid, &OmniList::selectionChanged, this, &GridView::selectionChanged);
    connect(m_grid, &OmniList::itemActivated, this, &GridView::itemActivated);
  }
};

class FormView : public SimpleView {
public:
  FormView(QWidget *parent = nullptr) : SimpleView(parent) {}

  bool needsGlobalStatusBar() const override { return true; }
  bool supportsSearch() const override { return false; }
};
