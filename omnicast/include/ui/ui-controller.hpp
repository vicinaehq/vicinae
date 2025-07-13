#pragma once
#include "action-panel/action-panel.hpp"
#include "argument.hpp"
#include "command.hpp"
#include "common.hpp"
#include "omni-icon.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/alert.hpp"
#include "ui/status_bar.hpp"
#include "ui/toast.hpp"
#include "ui/top_bar.hpp"
#include <functional>
#include <qlogging.h>
#include <qobject.h>
#include <qwidget.h>
#include <ranges>

class BaseView;

// general top level UI operation mediator
// Anything in omnicast can ask to push a new view and the signal will be handled by the main AppWindow
class UIController : public QObject {
  Q_OBJECT

  KeyboardShortcutModel DEFAULT_ACTION_PANEL_SHORTCUT = {.key = "B", .modifiers = {"ctrl"}};
  BaseView *m_view = nullptr;
  StatusBar *m_statusBar = nullptr;
  TopBar *m_topBar = nullptr;

  struct ViewState {
    BaseView *sender = nullptr;
    struct {
      QString title;
      OmniIconUrl icon;
    } navigation;
    QString placeholderText;
    QString text;
    QWidget *searchAccessory = nullptr;
    struct {
      std::vector<std::pair<QString, QString>> values;
      std::optional<CompleterData> data;
    } completer;
    ActionPanelV2Widget *actionPanel = nullptr;
    bool isLoading = false;
    bool supportsSearch = true;
    bool needsTopBar = true;
    bool needsStatusBar = true;
  };

  struct CommandSnapshot {
    QStack<ViewState> viewStack;
    std::unique_ptr<CommandContext> command;
  };

  std::vector<CommandSnapshot> m_commandStack;

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

  void handleActionButtonClick();
  void handleCurrentActionButtonClick();

  void handleCompleterArgumentsChanged(const std::vector<std::pair<QString, QString>> &values);

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
    connect(m_statusBar, &StatusBar::actionButtonClicked, this, &UIController::handleActionButtonClick);
    connect(m_statusBar, &StatusBar::currentActionButtonClicked, this,
            &UIController::handleCurrentActionButtonClick);
  }

  void activateCompleter(const ArgumentList &args, const OmniIconUrl &icon) {
    CompleterData completer{
        .iconUrl = icon,
        .arguments = args,
    };

    m_stateStack.back().completer = {.data = completer};
    m_topBar->activateCompleter(completer);
  }

  void destroyCompleter() {
    m_topBar->destroyCompleter();
    m_stateStack.back().completer = {};
  }

  auto arguments() { return m_topBar->m_completer->collect(); }
  auto argumentValues() {
    return arguments() | std::views::transform([](auto &&pair) { return pair.second; }) |
           std::ranges::to<std::vector>();
  }

  void replaceView(BaseView *previous, BaseView *next);
  void replaceCurrentView(BaseView *next) { replaceView(topView(), next); }

  void launchCommand(const QString &cmdId, const LaunchProps &opts = {});
  void launchCommand(const std::shared_ptr<AbstractCmd> &cmd, const LaunchProps &opts = {});

  void setToast(const QString &title, ToastPriority priority = ToastPriority::Success) const {
    emit showToastRequested(title, priority);
  }
  void popView();
  void setTopView(BaseView *view) { m_view = view; }
  BaseView *topView() const {
    if (m_stateStack.empty()) return nullptr;

    BaseView *view = m_stateStack.back().sender;

    return view;
  }

  QString navigationTitle() const { return m_stateStack.back().navigation.title; }
  OmniIconUrl navigationIcon() const { return m_stateStack.back().navigation.icon; }

  void setNavigationTitle(const QString &title) {
    m_stateStack.back().navigation.title = title;
    emit navigationTitleChanged(title);
  }

  void setNavigationIcon(const OmniIconUrl &icon) {
    m_stateStack.back().navigation.icon = icon;
    emit navigationIconChanged(icon);
  }

  void setSearchText(const QString &text) {
    m_stateStack.back().text = text;
    emit searchTextChanged(text);
  }

  void setSearchPlaceholderText(const QString &placeholderText) {
    m_stateStack.back().placeholderText = placeholderText;
    emit searchTextPlaceholderChanged(placeholderText);
  }

  QString searchText(BaseView *sender) const {
    if (auto state = findViewState(sender)) { return state->text; }
    return {};
  }

  QString searchText() const { return m_stateStack.back().text; }

  void setNavigation(BaseView *sender, const QString &title, const OmniIconUrl &icon) {
    if (sender == topView()) { m_statusBar->setNavigation(title, icon); }

    qCritical() << "set navigation" << title;
    updateViewState(sender, [&](ViewState &state) {
      state.navigation.title = title;
      state.navigation.icon = icon;
    });
  }

  void applyViewState(const ViewState &state);

  void executeAction(AbstractAction *action);
  void executeDefaultAction();

  void clearActionPanel() {
    qCritical() << "clear action panel";
    m_statusBar->setActionButtonVisibility(false);
    m_statusBar->setCurrentActionButtonVisibility(false);
  }

  void setActionPanel(ActionPanelV2Widget *panel) {
    m_statusBar->setActionButton("Actions", KeyboardShortcutModel{.key = "return"});

    auto actions = panel->actions();
    auto primaryAction = panel->primaryAction();

    m_statusBar->setActionButtonVisibility(!actions.empty() && (!primaryAction || actions.size() > 1));
    m_statusBar->setCurrentActionButtonVisibility(primaryAction);

    if (auto action = panel->primaryAction()) {
      m_statusBar->setCurrentAction(action->title(),
                                    action->shortcut.value_or(KeyboardShortcutModel{.key = "return"}));
      m_statusBar->setActionButton("Actions", defaultActionPanelShortcut());
    } else {
      m_statusBar->setActionButton("Actions", KeyboardShortcutModel{.key = "return"});
    }
  }

  void handleTextEdited(const QString &text);

  void setAlert(AlertWidget *alert) { emit alertRequested(alert); }

  void setTopBar(TopBar *bar) {
    m_topBar = bar;
    m_topBar->input->installEventFilter(this);
    connect(m_topBar->input, &QLineEdit::textEdited, this, &UIController::handleTextEdited);
    connect(m_topBar->input, &SearchBar::pop, this, &UIController::popView);
    connect(m_topBar, &TopBar::argumentsChanged, this, &UIController::handleCompleterArgumentsChanged);
  }

  void setLoading(bool loading) {
    m_stateStack.back().isLoading = loading;
    emit loadingChanged(loading);
  }

  void setSearchAccessory(QWidget *accessory) {
    m_stateStack.back().searchAccessory = accessory;
    emit searchAccessoryChanged(accessory);
  }

  void setActionPanelWidget(BaseView *sender, ActionPanelV2Widget *panel);
  void openSettings() const { emit openSettingsRequested(); }
  void updateCurrentView() { emit currentViewChanged(topView()); }

signals:
  void currentViewChanged(BaseView *view) const;
  void openSettingsRequested() const;
  void popViewRequested() const;
  void closeWindowRequested() const;
  void launchCommandRequested(const std::shared_ptr<AbstractCmd> &cmd) const;
  void launchCommandRequestedById(const QString &id) const;
  void showHUDRequested(const QString &title) const;
  void showToastRequested(const QString &title, ToastPriority priority) const;
  void replaceViewRequested(BaseView *previous, BaseView *next) const;

  void searchTextChanged(const QString &text) const;
  void searchTextPlaceholderChanged(const QString &text) const;
  void navigationTitleChanged(const QString &text) const;
  void navigationIconChanged(const OmniIconUrl &icon) const;
  void viewStackSizeChanged(size_t size) const;
  void loadingChanged(bool value) const;
  void searchAccessoryChanged(QWidget *widget) const;
  void actionPanelStateChanged(ActionPanelV2Widget *widget) const;

  void popViewCompleted() const;
  void alertRequested(AlertWidget *alert) const;
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
  // void setSearchText(const QString &text) { m_controller.setSearchText(&m_view, text); }
  // void setSearchPlaceholderText(const QString &text) { m_controller.setSearchPlaceholderText(&m_view,
  // text); }
  QString searchText() const { return m_controller.searchText(); }
  void setNavigation(const QString &title, const OmniIconUrl &url) {
    m_controller.setNavigation(&m_view, title, url);
  }
  void activateCompleter(const ArgumentList &args, const OmniIconUrl &url) {
    m_controller.activateCompleter(args, url);
  }
  void destroyCompleter() { m_controller.destroyCompleter(); }
  void setSearchAccessory(QWidget *widget) { m_controller.setSearchAccessory(widget); }
  void setLoading(bool value) { m_controller.setLoading(value); }
  void setActionPanelWidget(ActionPanelV2Widget *widget) {
    m_controller.setActionPanelWidget(&m_view, widget);
  }
  void hideSearch() {}

  UIViewController(UIController *controller, BaseView *view) : m_view(*view), m_controller(*controller) {}
};
