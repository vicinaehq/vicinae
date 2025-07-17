#pragma once
#include "action-panel/action-panel.hpp"
#include "argument.hpp"
#include "common.hpp"
#include "navigation-controller.hpp"
#include "omni-icon.hpp"
#include "service-registry.hpp"
#include <libqalculate/Calculator.h>
#include <qevent.h>
#include <qnamespace.h>
#include "ui/action-pannel/action.hpp"
#include "ui/empty-view.hpp"
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
#include <qstackedwidget.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class BaseView : public QWidget {
  bool m_initialized = false;
  ApplicationContext *m_ctx = nullptr;
  const BaseView *m_navProxy = this;

public:
  void createInitialize() {
    if (m_initialized) return;

    initialize();
    m_initialized = true;
  }
  bool isInitialized() { return m_initialized; }

  /**
   * Forward navigation opercontext()ations to `other` instead of this view.
   * Allows to nest view with only one effectively having navigation responsability.
   * In most cases, you should not use this. This is mainly used for extensions.
   */
  void setProxy(BaseView *proxy) {
    m_navProxy = proxy;
    setContext(proxy->context());
  }

  void setActions(std::unique_ptr<ActionPanelState> actions) {
    if (!m_ctx) return;
    m_ctx->navigation->setActions(std::move(actions), m_navProxy);
  }

  /**
   * Whether to show the search bar for this view. Calling setSearchText or searchText() is still
   * valid but will always return the empty string.
   */
  virtual bool supportsSearch() const { return true; }

  void executePrimaryAction() { m_ctx->navigation->executePrimaryAction(); }

  virtual bool needsGlobalStatusBar() const { return true; }
  virtual bool needsGlobalTopBar() const { return true; }

  void setActionPanelWidget(ActionPanelV2Widget *widget) { /**/ }

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

  OmniIconUrl navigationIcon() const { return BuiltinOmniIconUrl("question-mark-circle"); }

  QString navigationTitle() const {
    if (m_ctx) { return m_ctx->navigation->navigationTitle(m_navProxy); }
    return QString();
  }

  void setSearchAccessory(QWidget *accessory) {
    if (!m_ctx) return;

    m_ctx->navigation->setSearchAccessory(accessory, m_navProxy);
  }

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
    // m_uiController->activateCompleter(args, icon);
  }

  void setUIController(std::unique_ptr<UIViewController> controller) {
    // m_uiController = std::move(controller);
  }

  void setContext(ApplicationContext *ctx) { m_ctx = ctx; }

  /**
   * The entire application context.
   * You normally do not need to use this directly. Use the helper methods instead.
   * Note that the returned context is only valid if the view is tracked by the navigation
   * controller. A view not (yet) tracked will have this function return a null pointer.
   */
  ApplicationContext *context() const { return m_ctx; }

  void destroyCompleter() { /*m_uiController->destroyCompleter();*/ }

  virtual QWidget *searchBarAccessory() const { return nullptr; }

  QString searchPlaceholderText() const { /* todo: implemement */ return ""; }

  void setSearchPlaceholderText(const QString &value) const {
    if (!m_ctx) return;
    m_ctx->navigation->setSearchPlaceholderText(value, m_navProxy);
  }

  void clearSearchAccessory() { m_ctx->navigation->clearSearchAccessory(m_navProxy); }

  void setTopBarVisiblity(bool visible) {
    if (!m_ctx) return;
    m_ctx->navigation->setHeaderVisiblity(visible, m_navProxy);
  }

  void setSearchVisibility(bool visible) {
    if (!m_ctx) return;
    m_ctx->navigation->setSearchVisibility(visible, m_navProxy);
  }

  void setStatusBarVisiblity(bool visible) {
    if (!m_ctx) return;
    m_ctx->navigation->setStatusBarVisibility(visible, m_navProxy);
  }

  void clearSearchText() { setSearchText(""); }

  /**
   * The current search text for this view. If not applicable, do not implement.
   */
  QString searchText() const {
    if (!m_ctx) return QString();
    return m_ctx->navigation->searchText(m_navProxy);
  }

  /**
   * Set the search text for the current view, if applicable
   */
  void setSearchText(const QString &value) {
    if (!m_ctx) return;
    return m_ctx->navigation->setSearchText(value, m_navProxy);
  }

  virtual ActionPanelV2Widget *actionPanel() const { return nullptr; }

  /**
   * Allows the view to filter input from the main search bar before the input itself
   * processes it.
   * For instance, this allows a list view to capture up and down events to move the position in the list.
   * Or, as an example that actually modifies the input behaviour, a grid list (with horizontal controls)
   * can repurpose the left and right keys to navigate the list, while they would normally move the text
   * cursor.
   *
   * In typical QT event filter fashion, this function should return false if the key is left for the input
   * to handle, or true if it needs to be ignored.
   *
   */
  virtual bool inputFilter(QKeyEvent *event) { return false; }

  /**
   * Set the navigation icon, if applicable
   */
  virtual void setNavigationIcon(const OmniIconUrl &icon) {
    // m_uiController->setNavigationIcon(icon);
  }

  void setNavigation(const QString &title, const OmniIconUrl &icon) { setNavigationTitle(title); }

  void setNavigationTitle(const QString &title) {
    if (!m_ctx) return;
    return m_ctx->navigation->setNavigationTitle(title, m_navProxy);
  }

  void setLoading(bool value) {
    // m_uiController->setLoading(value);
  }

  /**
   * The dynamic arguments for this view. Used by some actions.
   */
  virtual std::vector<QString> argumentValues() const { return {}; }

  BaseView(QWidget *parent = nullptr) : QWidget(parent) {}
};

class SimpleView : public BaseView {
protected:
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

  void onDeactivate() override { m_actionPannelV2->reset(); }

  void setupUI(QWidget *centerWidget) {
    QVBoxLayout *m_layout = new QVBoxLayout;
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);
    // m_layout->addWidget(m_topBar);
    m_layout->addWidget(centerWidget, 1);
    setLayout(m_layout);
  }

public:
  std::vector<QString> argumentValues() const override {
    return {};
    // return m_topBar->m_completer->collect() | std::views::transform([](const auto &p) { return p.second;
    // }) | std::ranges::to<std::vector>();
  }

  void clearSearchText() { setSearchText(""); }

  void initialize() override {}

public:
  ActionPanelV2Widget *actionPanel() const override { return m_actionPannelV2; }

  SimpleView(QWidget *parent = nullptr) : BaseView(parent) {
    // m_topBar->showBackButton();
  }
};

class ListView : public SimpleView {
  SplitDetailWidget *m_split = new SplitDetailWidget(this);
  QStackedWidget *m_content = new QStackedWidget(this);
  EmptyViewWidget *m_emptyView = new EmptyViewWidget(this);

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

    virtual std::unique_ptr<ActionPanelState> newActionPanel(ApplicationContext *ctx) const {
      return std::make_unique<ActionPanelState>();
    }

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
        // activateCompleter(completer->arguments, completer->iconUrl);
      } else {
        // destroyCompleter();
      }

      // TODO: only expect suffix and automatically use command name from prefix
      if (auto navigation = nextItem->navigationTitle(); !navigation.isEmpty()) {
        // setNavigationTitle(QString("%1 - %2").arg(m_baseNavigationTitle).arg(navigation));
        //
      }

      context()->navigation->setActions(nextItem->newActionPanel(context()));

      /*
  if (auto panel = nextItem->actionPanel()) {
    m_actionPannelV2->setView(panel);
  } else {
    m_actionPannelV2->hide();
    m_actionPannelV2->popToRoot();
  }
      */

    } else {
      m_split->setDetailVisibility(false);
      // destroyCompleter();
      m_actionPannelV2->popToRoot();
      m_actionPannelV2->close();
    }

    itemSelected(next);
  }

  void regenerateActions() {
    if (auto selected = m_list->selected()) {
      if (auto nextItem = dynamic_cast<const Actionnable *>(selected)) {
        if (auto panel = nextItem->actionPanel()) { m_actionPannelV2->setView(panel); }
      }
    }
  }

  virtual void itemActivated(const OmniList::AbstractVirtualItem &item) { executePrimaryAction(); }

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
    m_content->addWidget(m_split);
    m_content->addWidget(m_emptyView);
    m_content->setCurrentWidget(m_list);

    m_emptyView->setTitle("No results");
    m_emptyView->setDescription("No results matching your search. You can try to refine your search.");
    m_emptyView->setIcon(BuiltinOmniIconUrl("magnifying-glass"));

    m_split->setMainWidget(m_list);
    setupUI(m_content);
    connect(m_list, &OmniList::modelChanged, this, &ListView::modelChanged);
    connect(m_list, &OmniList::selectionChanged, this, &ListView::selectionChanged);
    connect(m_list, &OmniList::itemActivated, this, &ListView::itemActivated);
    connect(m_list, &OmniList::itemRightClicked, this, &ListView::itemRightClicked);
    connect(m_list, &OmniList::virtualHeightChanged, this, [this](int height) {
      if (m_list->items().empty()) {
        auto ui = ServiceRegistry::instance()->UI();

        // ui->destroyCompleter();
        m_content->setCurrentWidget(m_emptyView);
        return;
      }

      m_content->setCurrentWidget(m_split);
    });
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

  virtual QString rootNavigationTitle() const { return ""; }

  void onActivate() override {
    if (auto selection = m_grid->selected()) {
      if (auto nextItem = dynamic_cast<const Actionnable *>(selection)) { applyActionnable(nextItem); }
    }
  }

  void applyActionnable(const Actionnable *actionnable) {
    if (auto navigation = actionnable->navigationTitle(); !navigation.isEmpty()) {
      setNavigationTitle(QString("%1 - %2").arg(rootNavigationTitle()).arg(navigation));
    }
  }

protected:
  OmniGrid *m_grid = new OmniGrid();
  QStackedWidget *m_content = new QStackedWidget(this);
  EmptyViewWidget *m_emptyView = new EmptyViewWidget(this);

  virtual void selectionChanged(const OmniList::AbstractVirtualItem *next,
                                const OmniList::AbstractVirtualItem *previous) {
    if (!next) {
      setNavigationTitle(rootNavigationTitle());
      return;
    }

    if (auto nextItem = dynamic_cast<const Actionnable *>(next)) {
      applyActionnable(nextItem);
    } else {
      context()->navigation->clearActions(this);
      destroyCompleter();
    }
  }

  virtual void itemActivated(const OmniList::AbstractVirtualItem &item) { executePrimaryAction(); }

public:
  GridView(QWidget *parent = nullptr) : SimpleView(parent) {
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
      if (m_grid->items().empty()) {
        m_content->setCurrentWidget(m_emptyView);
        return;
      }

      m_content->setCurrentWidget(m_grid);
    });
  }
};

class FormView : public SimpleView {
public:
  FormView(QWidget *parent = nullptr) : SimpleView(parent) {}

  bool needsGlobalStatusBar() const override { return true; }
  bool supportsSearch() const override { return false; }
};
