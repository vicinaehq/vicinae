#pragma once
#include "action-panel/action-panel.hpp"
#include "argument.hpp"
#include "command.hpp"
#include "common.hpp"
#include "omni-icon.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/alert.hpp"
#include "ui/toast.hpp"
#include "ui/top_bar.hpp"
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

  struct ViewState {
    BaseView *sender = nullptr;
    struct {
      QString title;
      OmniIconUrl icon;
    } navigation;
    QString placeholderText;
    QString text;
    std::unique_ptr<QWidget> searchAccessory;
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

  struct CommandFrame {
    std::unique_ptr<CommandContext> command;
    std::vector<ViewState> viewStack;

    CommandFrame(CommandContext *ctx) : command(ctx) {}
    ~CommandFrame() {
      qDebug() << "~CommandFrame - unloaded";
      command->unload();
    }
  };

  std::vector<std::unique_ptr<CommandFrame>> m_commandFrames;

  KeyboardShortcutModel defaultActionPanelShortcut() { return DEFAULT_ACTION_PANEL_SHORTCUT; }

  void applyViewState(const ViewState &state);

  ViewState *topState() {
    if (m_commandFrames.empty()) return nullptr;

    for (auto it = m_commandFrames.rbegin(); it != m_commandFrames.rend(); ++it) {
      if (!(*it)->viewStack.empty()) return &(*it)->viewStack.back();
    }

    return nullptr;
  }

  const ViewState *topState() const {
    if (m_commandFrames.empty()) return nullptr;

    for (auto it = m_commandFrames.rbegin(); it != m_commandFrames.rend(); ++it) {
      if (!(*it)->viewStack.empty()) return &(*it)->viewStack.back();
    }

    return nullptr;
  }

public:
  UIController() {}
  ~UIController() {}

  void closeWindow() { emit closeWindowRequested(); }
  void popToRoot() {}
  void showHUD(const QString &title) { emit showHUDRequested(title); }
  void pushView(BaseView *view, const PushViewOptions &opts = {});
  void clearActionPanel() {}

  bool canPop() const { return totalViewCount() > 1 || !searchText().isEmpty(); }

  void activateCompleter(const ArgumentList &args, const OmniIconUrl &icon) {
    CompleterData completer{
        .iconUrl = icon,
        .arguments = args,
    };

    topState()->completer = {.data = completer};
    emit completerActivated(args, icon);
  }

  void destroyCompleter() {
    topState()->completer = {};
    emit completerDestroyed();
  }

  auto arguments() { return std::vector<std::pair<QString, QString>>{}; }
  auto argumentValues() {
    return arguments() | std::views::transform([](auto &&pair) { return pair.second; }) |
           std::ranges::to<std::vector>();
  }

  void launchCommand(const QString &cmdId, const LaunchProps &opts = {});
  void launchCommand(const std::shared_ptr<AbstractCmd> &cmd, const LaunchProps &opts = {});

  void setToast(const QString &title, ToastPriority priority = ToastPriority::Success) const {
    emit showToastRequested(title, priority);
  }
  void popView();
  void setTopView(BaseView *view) { m_view = view; }
  BaseView *topView() {
    if (auto state = topState()) return state->sender;

    return nullptr;
  }

  QString navigationTitle() const;
  OmniIconUrl navigationIcon() const;
  void setNavigationTitle(const QString &title);
  void setNavigationIcon(const OmniIconUrl &icon);
  void setSearchText(const QString &text);
  void setSearchPlaceholderText(const QString &placeholderText);
  QString searchText() const;

  void unloadHangingCommands();

  bool isRootSearch() { return m_commandFrames.size() == 1 && m_commandFrames.back()->viewStack.size() == 1; }

  size_t totalViewCount() const {
    size_t count = 0;

    for (const auto &cmd : m_commandFrames)
      count += cmd->viewStack.size();

    return count;
  }

  void executeAction(AbstractAction *action);
  void executeDefaultAction();

  void setAlert(AlertWidget *alert) { emit alertRequested(alert); }

  void setLoading(bool loading) {
    topState()->isLoading = loading;
    emit loadingChanged(loading);
  }

  ViewState *findViewState(const BaseView *view) {
    for (const auto &frame : m_commandFrames) {
      if (auto it =
              std::ranges::find_if(frame->viewStack, [&](auto &&state) { return state.sender == view; });
          it != frame->viewStack.end()) {
        return &*it;
      }
    }

    return nullptr;
  }

  ViewState *findViewState(const BaseView *view) const {
    for (const auto &frame : m_commandFrames) {
      if (auto it =
              std::ranges::find_if(frame->viewStack, [&](auto &&state) { return state.sender == view; });
          it != frame->viewStack.end()) {
        return &*it;
      }
    }

    return nullptr;
  }

  void setSearchAccessory(QWidget *accessory) {
    auto state = topState();

    if (state->searchAccessory.get() != accessory) { state->searchAccessory.reset(accessory); }
    emit searchAccessoryChanged(accessory);
  }

  void openSettings() const { emit openSettingsRequested(); }
  void updateCurrentView() { emit currentViewChanged(topView()); }

  void setActionPanelWidget(ActionPanelV2Widget *widget);
  void setActionPanelWidget(ActionPanelV2Widget *widget, const BaseView *view);

  void setLoading(const BaseView *view, bool value);
  QString searchText(const BaseView *view) const;
  void setSearchText(const BaseView *view, const QString &text);
  QString searchPlaceholderText(const BaseView *view) const;
  void setSearchPlaceholderText(const BaseView *view, const QString &text);
  QString navigationTitle(const BaseView *view) const;
  void setNavigationTitle(const BaseView *view, const QString &title);
  void setNavigationIcon(const BaseView *view, const OmniIconUrl &icon);

  void setSearchVisiblity(const BaseView *sender, bool visible);
  void setTopBarVisiblity(const BaseView *sender, bool visible);
  void setStatusBarVisibility(const BaseView *sender, bool visible);

signals:
  void currentViewChanged(BaseView *view) const;
  void openSettingsRequested() const;
  void closeWindowRequested() const;
  void showHUDRequested(const QString &title) const;
  void showToastRequested(const QString &title, ToastPriority priority) const;

  void searchTextChanged(const QString &text) const;
  void searchTextPlaceholderChanged(const QString &text) const;
  void navigationTitleChanged(const QString &text) const;
  void navigationIconChanged(const OmniIconUrl &icon) const;
  void viewStackSizeChanged(size_t size) const;
  void loadingChanged(bool value) const;
  void searchAccessoryChanged(QWidget *widget) const;
  void actionPanelStateChanged(ActionPanelV2Widget *widget) const;
  void actionPanelOpenChanged(bool value) const;
  void actionExecutionRequested(AbstractAction *action) const;
  void completerActivated(const ArgumentList &args, const OmniIconUrl &icon) const;
  void completerDestroyed() const;
  bool searchVisiblityChanged(bool visible);
  bool topBarVisiblityChanged(bool visible);
  bool statusBarVisiblityChanged(bool visible);

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
  void setSearchText(const QString &text) { m_controller.setSearchText(&m_view, text); }
  void setSearchPlaceholderText(const QString &text) { m_controller.setSearchPlaceholderText(&m_view, text); }

  void setNavigationTitle(const QString &title) { m_controller.setNavigationTitle(&m_view, title); }
  void setNavigationIcon(const OmniIconUrl &icon) { m_controller.setNavigationIcon(&m_view, icon); }

  QString searchText() const { return m_controller.searchText(); }
  QString searchPlaceholderText() const { return m_controller.searchPlaceholderText(&m_view); }

  QString navigationTitle() const { return m_controller.navigationTitle(); }

  OmniIconUrl navigationIcon() const { return m_controller.navigationIcon(); }

  void setSearchVisiblity(bool visible) { return m_controller.setSearchVisiblity(&m_view, visible); }

  void setTopBarVisiblity(bool visible) { return m_controller.setTopBarVisiblity(&m_view, visible); }

  void setStatusBarVisibility(bool visible) { return m_controller.setStatusBarVisibility(&m_view, visible); }

  void setNavigation(const QString &title, const OmniIconUrl &url) {
    // m_controller.setNavigation(&m_view, title, url);
  }
  void activateCompleter(const ArgumentList &args, const OmniIconUrl &url) {
    m_controller.activateCompleter(args, url);
  }
  void destroyCompleter() { m_controller.destroyCompleter(); }
  void setSearchAccessory(QWidget *widget) { m_controller.setSearchAccessory(widget); }
  void setLoading(bool value) { m_controller.setLoading(&m_view, value); }
  void setActionPanelWidget(ActionPanelV2Widget *widget) {
    m_controller.setActionPanelWidget(widget, &m_view);
  }
  void hideSearch() {}

  UIViewController(UIController *controller, BaseView *view) : m_view(*view), m_controller(*controller) {}
};
