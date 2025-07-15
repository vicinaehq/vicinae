#include "base-view.hpp"
#include "common.hpp"
#include "extension/extension-command.hpp"
#include <qevent.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qobjectdefs.h>
#include "extension/missing-extension-preference-view.hpp"
#include "ui/ui-controller.hpp"

void UIController::launchCommand(const QString &id, const LaunchProps &opts) {
  auto commandDb = ServiceRegistry::instance()->commandDb();

  if (auto command = commandDb->findCommand(id)) { launchCommand(command->command, opts); }
}

QString UIController::navigationTitle() const { return topState()->navigation.title; }

OmniIconUrl UIController::navigationIcon() const { return topState()->navigation.icon; }

void UIController::setNavigationTitle(const QString &title) {
  topState()->navigation.title = title;
  emit navigationTitleChanged(title);
}

void UIController::setNavigationIcon(const OmniIconUrl &icon) {
  topState()->navigation.icon = icon;
  emit navigationIconChanged(icon);
}

void UIController::launchCommand(const std::shared_ptr<AbstractCmd> &cmd, const LaunchProps &opts) {
  auto itemId = QString("extension.%1").arg(cmd->uniqueId());
  auto manager = ServiceRegistry::instance()->rootItemManager();
  auto preferences = manager->getMergedItemPreferences(itemId);
  auto preferenceValues = manager->getPreferenceValues(itemId);

  for (const auto &preference : preferences) {
    if (preference.required() && !preferenceValues.contains(preference.name()) &&
        preference.defaultValue().isUndefined()) {
      if (cmd->type() == CommandType::CommandTypeExtension) {
        auto extensionCommand = std::static_pointer_cast<ExtensionCommand>(cmd);

        pushView(new MissingExtensionPreferenceView(extensionCommand, preferences, preferenceValues),
                 {.navigation = NavigationStatus{.title = cmd->name(), .iconUrl = cmd->iconUrl()}});
        return;
      }

      qDebug() << "MISSING PREFERENCE" << preference.title();
    }
  }

  unloadHangingCommands();

  auto ctx = cmd->createContext(cmd);

  if (!ctx) { return; }

  if (cmd->isNoView() && cmd->type() == CommandType::CommandTypeBuiltin) {
    qCritical() << "Running no view command";
    ctx->load(opts);
  } else {
    m_commandFrames.push_back(std::make_unique<CommandFrame>(ctx));
    ctx->load(opts);
  }
}

void UIController::pushView(BaseView *view, const PushViewOptions &opts) {
  qCritical() << "push view";

  if (auto state = topState()) {
    if (auto panel = state->actionPanel) {
      disconnect(panel, nullptr, this, nullptr);
      panel->close();
    }

    if (auto accessory = view->searchBarAccessory()) {
      setSearchAccessory(nullptr);
      accessory->hide();
    }

    destroyCompleter();
    view->deactivate();
    view->hide();
    view->clearFocus();
  }

  ViewState state;

  state.sender = view;
  state.supportsSearch = view->supportsSearch();
  state.needsTopBar = view->needsGlobalTopBar();
  state.needsStatusBar = view->needsGlobalStatusBar();
  state.actionPanel = view->actionPanel();
  state.searchAccessory = std::unique_ptr<QWidget>(view->searchBarAccessory());

  if (m_commandFrames.empty()) {
    qDebug() << "AppWindow::pushView called with empty command stack";
    return;
  }

  auto &currentCommand = m_commandFrames.at(m_commandFrames.size() - 1);

  currentCommand->viewStack.emplace_back(std::move(state));
  applyViewState(state);

  if (auto navigation = opts.navigation) {
    qDebug() << "set navigation";
    setNavigationTitle(navigation->title);
    setNavigationIcon(navigation->iconUrl);
  }

  emit currentViewChanged(view);
  emit viewStackSizeChanged(totalViewCount());

  view->show();
  view->createInitialize();
  view->onActivate();
}

QString UIController::searchText() const {
  if (auto state = topState()) return state->text;
  return {};
}

void UIController::setSearchPlaceholderText(const QString &placeholderText) {
  topState()->placeholderText = placeholderText;
  emit searchTextPlaceholderChanged(placeholderText);
}

void UIController::setSearchText(const QString &text) {
  topState()->text = text;

  if (auto view = topView()) { view->textChanged(text); }

  emit searchTextChanged(text);
}

void UIController::executeAction(AbstractAction *action) { emit actionExecutionRequested(action); }

void UIController::executeDefaultAction() {
  if (m_commandFrames.empty()) return;

  auto state = topState();

  if (auto panel = state->actionPanel) {
    if (auto action = panel->primaryAction()) {
      executeAction(action);
    } else {
      panel->show();
    }
  }
}

void UIController::unloadHangingCommands() {
  while (!m_commandFrames.empty() && m_commandFrames.back()->viewStack.empty()) {
    m_commandFrames.pop_back();
  }
}

void UIController::popView() {
  unloadHangingCommands();

  if (isRootSearch()) {
    if (!canPop()) {
      closeWindow();
      return;
    }

    setSearchText("");
    return;
  }

  if (auto state = topState()) {
    state->sender->deactivate();
    state->sender->deleteLater();

    if (auto &accessory = state->searchAccessory) { setSearchAccessory(nullptr); }
  }

  auto &activeCommand = m_commandFrames.at(m_commandFrames.size() - 1);

  if (activeCommand->viewStack.empty()) {
    qDebug() << "active command view stack empty";
    return;
  }

  activeCommand->viewStack.pop_back();

  if (activeCommand->viewStack.empty()) {
    m_commandFrames.pop_back();
    qDebug() << "popping cmd stack now" << m_commandFrames.size();
  }

  auto state = topState();

  if (!state) return;

  applyViewState(*state);
  state->sender->activate();

  emit currentViewChanged(state->sender);
  emit viewStackSizeChanged(totalViewCount());
  emit popViewCompleted();
}

void UIController::applyViewState(const ViewState &state) {
  qCritical() << "restore navigation" << state.navigation.title;

  setNavigationTitle(state.navigation.title);
  setNavigationIcon(state.navigation.icon);
  emit searchTextChanged(state.text);
  setSearchPlaceholderText(state.placeholderText);
  setSearchAccessory(state.searchAccessory.get());
  setLoading(state.isLoading);

  if (auto data = state.completer.data) {
    activateCompleter(data->arguments, data->iconUrl);
  } else {
    destroyCompleter();
  }

  if (auto panel = state.actionPanel) {
    disconnect(panel, nullptr, this, nullptr);
    connect(panel, &ActionPanelV2Widget::openChanged, this, &UIController::actionPanelOpenChanged);
    connect(panel, &ActionPanelV2Widget::actionsChanged, this,
            [this, panel]() { emit actionPanelStateChanged(panel); });
    connect(panel, &ActionPanelV2Widget::actionActivated, this, &UIController::executeAction);
    emit actionPanelStateChanged(panel);
  } else {
    // clearActionPanel();
  }
}

void UIController::setSearchText(const BaseView *view, const QString &text) {
  if (auto state = findViewState(view)) { state->text = text; }
  if (view == topView()) { setSearchText(text); }
}

void UIController::setSearchPlaceholderText(const BaseView *view, const QString &text) {
  if (auto state = findViewState(view)) { state->placeholderText = text; }
  if (view == topView()) { setSearchPlaceholderText(text); }
}

QString UIController::navigationTitle(const BaseView *view) const {
  if (auto state = findViewState(view)) { return state->navigation.title; }

  return QString();
}

void UIController::setNavigationTitle(const BaseView *view, const QString &title) {
  if (auto state = findViewState(view)) { state->navigation.title = title; }
  if (view == topView()) { setNavigationTitle(title); }
}

void UIController::setNavigationIcon(const BaseView *view, const OmniIconUrl &icon) {
  if (auto state = findViewState(view)) { state->navigation.icon = icon; }
  if (view == topView()) { setNavigationIcon(icon); }
}

QString UIController::searchText(const BaseView *view) const {
  if (auto state = findViewState(view)) { return state->text; }

  return QString();
}

QString UIController::searchPlaceholderText(const BaseView *view) const {
  if (auto state = findViewState(view)) { return state->placeholderText; }

  return QString();
}

void UIController::setSearchVisiblity(const BaseView *view, bool visible) {
  if (auto state = findViewState(view)) { state->supportsSearch = visible; }

  emit searchVisiblityChanged(visible);
}

void UIController::setTopBarVisiblity(const BaseView *sender, bool visible) {
  if (auto state = findViewState(sender)) { state->needsTopBar = visible; }
  emit topBarVisiblityChanged(visible);
}

void UIController::setStatusBarVisibility(const BaseView *sender, bool visible) {
  if (auto state = findViewState(sender)) { state->needsStatusBar = visible; }
  emit statusBarVisiblityChanged(visible);
}

void UIController::setActionPanelWidget(ActionPanelV2Widget *widget) {
  if (!widget) return;

  if (auto state = topState()) {
    qCritical() << "setActionPanelWidget";
    state->actionPanel = widget;

    disconnect(widget, nullptr, this, nullptr);
    connect(widget, &ActionPanelV2Widget::openChanged, this, &UIController::actionPanelOpenChanged);
    connect(widget, &ActionPanelV2Widget::actionsChanged, this,
            [this, widget]() { emit actionPanelStateChanged(widget); });
    connect(widget, &ActionPanelV2Widget::actionActivated, this, &UIController::executeAction);
    emit actionPanelStateChanged(widget);
  }
}

void UIController::setActionPanelWidget(ActionPanelV2Widget *widget, const BaseView *sender) {
  if (auto state = findViewState(sender)) { state->actionPanel = widget; }

  if (sender == topView()) {
    connect(widget, &ActionPanelV2Widget::openChanged, this, &UIController::actionPanelOpenChanged);
    connect(widget, &ActionPanelV2Widget::actionsChanged, this,
            [this, widget]() { emit actionPanelStateChanged(widget); });
    connect(widget, &ActionPanelV2Widget::actionActivated, this, &UIController::executeAction);
    emit actionPanelStateChanged(widget);
  }
}

void UIController::setLoading(const BaseView *view, bool value) {
  if (auto state = findViewState(view)) { state->isLoading = value; }
  if (view == topView()) { emit loadingChanged(value); }
}
