#pragma once
#include "action-panel/action-panel.hpp"
#include "common.hpp"
#include "omni-icon.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/status_bar.hpp"
#include "ui/toast.hpp"
#include "ui/top_bar.hpp"
#include <functional>
#include <qlogging.h>
#include <qobject.h>

class BaseView;

// general top level UI operation mediator
// Anything in omnicast can ask to push a new view and the signal will be handled by the main AppWindow
class UIController : public QObject {
  Q_OBJECT

  KeyboardShortcutModel DEFAULT_ACTION_PANEL_SHORTCUT = {.key = "B", .modifiers = {"ctrl"}};
  BaseView *m_view = nullptr;
  StatusBar *m_statusBar = nullptr;
  TopBar *m_topBar = nullptr;
  ActionPanelV2Widget *m_actionPanel = nullptr;

  struct ViewState {
    BaseView *sender = nullptr;
    struct {
      QString title;
      OmniIconUrl icon;
    } navigation;
    QString placeholderText;
    QString text;

    ActionPanelView *actionPanel = nullptr;
  };

  KeyboardShortcutModel defaultActionPanelShortcut() { return DEFAULT_ACTION_PANEL_SHORTCUT; }

  std::vector<ViewState> m_stateStack;

  bool updateViewState(BaseView *view, const std::function<void(ViewState &state)> &fn) {
    auto it = std::ranges::find_if(m_stateStack, [&](auto &&state) { return state.sender == view; });

    if (it != m_stateStack.end()) {
      fn(*it);
      return true;
    }

    return false;
  }

  const ViewState *findViewState(BaseView *view) const {
    auto it = std::ranges::find_if(m_stateStack, [&](auto &&state) { return state.sender == view; });

    if (it != m_stateStack.end()) return &*it;

    return nullptr;
  }

  bool eventFilter(QObject *watched, QEvent *event) override;

public:
  UIController() {}
  ~UIController() {}

  void closeWindow() { emit closeWindowRequested(); }
  void popToRoot() {
    while (m_stateStack.size() > 1) {
      popView();
    }
  }
  void showHUD(const QString &title) { emit showHUDRequested(title); }
  void pushView(BaseView *view, const PushViewOptions &opts = {});
  void setStatusBar(StatusBar *bar) {
    m_statusBar = bar;
    connect(m_statusBar, &StatusBar::actionButtonClicked, this, [this]() { m_actionPanel->show(); });
    connect(m_statusBar, &StatusBar::currentActionButtonClicked, this, [this]() { executeDefaultAction(); });
  }
  void replaceView(BaseView *previous, BaseView *next) { emit replaceViewRequested(previous, next); }
  void replaceCurrentView(BaseView *next) { emit replaceViewRequested(topView(), next); }
  void launchCommand(const std::shared_ptr<AbstractCmd> &cmd) const { emit launchCommandRequested(cmd); }
  void launchCommand(const QString &cmdId) const { /*emit launchCommandRequested(cmdId);*/ }
  void setToast(const QString &title, ToastPriority priority = ToastPriority::Success) const {
    emit showToastRequested(title, priority);
  }
  void popView();
  void setTopView(BaseView *view) { m_view = view; }
  BaseView *topView() const { return m_stateStack.back().sender; }

  void setNavigation(const QString &title, const OmniIconUrl &icon) { setNavigation(topView(), title, icon); }
  void setSearchText(const QString &text) { setSearchText(topView(), text); }
  void setSearchPlaceholderText(const QString &placeholderText) {
    setSearchPlaceholderText(topView(), placeholderText);
  }

  void setSearchText(BaseView *sender, const QString &text) {
    if (sender == topView()) { m_topBar->input->setText(text); }
    updateViewState(sender, [&](ViewState &state) { state.text = text; });
  }

  QString searchText(BaseView *sender, const QString &text) const {
    if (auto state = findViewState(sender)) { return state->text; }
    return {};
  }

  QString searchText() const { return m_topBar->input->text(); }

  void setSearchPlaceholderText(BaseView *sender, const QString &placeholderText) {
    if (sender == topView()) { m_topBar->input->setPlaceholderText(placeholderText); }
    updateViewState(sender, [&](ViewState &state) { state.placeholderText = placeholderText; });
  }

  void setNavigation(BaseView *sender, const QString &title, const OmniIconUrl &icon) {
    if (sender == topView()) { m_statusBar->setNavigation(title, icon); }

    qCritical() << "set navigation" << title;
    updateViewState(sender, [&](ViewState &state) {
      state.navigation.title = title;
      state.navigation.icon = icon;
    });
  }

  void applyViewState(const ViewState &state) {
    qCritical() << "restore navigation" << state.navigation.title;
    m_statusBar->setNavigationTitle(state.navigation.title);
    m_statusBar->setNavigationIcon(state.navigation.icon);

    m_topBar->input->setText(state.text);
    m_topBar->input->setPlaceholderText(state.placeholderText);

    if (state.actionPanel) { setActionPanel(state.actionPanel); }
  }

  void executeAction(AbstractAction *action) {
    action->execute();
    if (action->isSubmenu()) {
      if (auto panel = action->createSubmenu()) {
        m_actionPanel->pushView(panel);
        m_actionPanel->show();
        return;
      }
    }
    m_actionPanel->close();
  }

  void executeDefaultAction() {
    if (auto action = m_actionPanel->primaryAction()) {
      executeAction(action);
    } else {
      m_actionPanel->show();
    }
  }

  void clearActionPanel() {
    m_stateStack.back().actionPanel = nullptr;
    m_statusBar->setActionButtonVisibility(false);
    m_statusBar->setCurrentActionButtonVisibility(false);
    m_actionPanel->close();
  }

  void setActionPanel(ActionPanelView *panel) {
    m_stateStack.back().actionPanel = panel;

    m_actionPanel->setView(panel);
    m_statusBar->setActionButton("Actions", KeyboardShortcutModel{.key = "return"});

    auto actions = m_actionPanel->actions();
    auto primaryAction = m_actionPanel->primaryAction();

    m_statusBar->setActionButtonVisibility(!primaryAction || actions.size() > 1);
    m_statusBar->setCurrentActionButtonVisibility(primaryAction);

    if (auto action = m_actionPanel->primaryAction()) {
      m_statusBar->setCurrentAction(action->title(),
                                    action->shortcut.value_or(KeyboardShortcutModel{.key = "return"}));
      m_statusBar->setActionButton("Actions", defaultActionPanelShortcut());
    } else {
      m_statusBar->setActionButton("Actions", KeyboardShortcutModel{.key = "return"});
    }
  }

  void handleTextEdited(const QString &text);

  void setTopBar(TopBar *bar) {
    m_topBar = bar;
    m_topBar->input->installEventFilter(this);
    connect(m_topBar->input, &QLineEdit::textEdited, this, &UIController::handleTextEdited);
    connect(m_topBar->input, &SearchBar::pop, this, &UIController::popView);
  }

  void setActionPanelWidget(ActionPanelV2Widget *panel) {
    m_actionPanel = panel;
    connect(panel, &ActionPanelV2Widget::openChanged, this,
            [this](bool value) { m_statusBar->setActionButtonHighlight(value); });
    connect(panel, &ActionPanelV2Widget::actionActivated, this,
            [this](AbstractAction *action) { executeAction(action); });
  }

signals:
  void pushViewRequested(BaseView *view, const PushViewOptions &opts) const;
  void popViewRequested() const;
  void popToRootRequested() const;
  void closeWindowRequested() const;
  void launchCommandRequested(const std::shared_ptr<AbstractCmd> &cmd) const;
  void showHUDRequested(const QString &title) const;
  void showToastRequested(const QString &title, ToastPriority priority) const;
  void replaceViewRequested(BaseView *previous, BaseView *next) const;

  void popViewCompleted() const;
};

/**
 * Interface a particular view can use to interact with the UI controller.
 * This allows for proper segmentation of layout values.
 *
 */
class UIViewController {
  BaseView &m_view;
  UIController &m_controller;

public:
  void setSearchText(const QString &text) { m_controller.setSearchText(&m_view, text); }
  void setSearchPlaceholderText(const QString &text) { m_controller.setSearchPlaceholderText(&m_view, text); }
  void setNavigation(const QString &title, const OmniIconUrl &url) {
    m_controller.setNavigation(&m_view, title, url);
  }

  UIViewController(UIController *controller, BaseView *view) : m_view(*view), m_controller(*controller) {}
};
