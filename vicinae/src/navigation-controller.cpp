#include "navigation-controller.hpp"
#include "command-controller.hpp"
#include "extension/extension-command.hpp"
#include "root-search/extensions/extension-root-provider.hpp"
#include "service-registry.hpp"
#include "overlay-controller/overlay-controller.hpp"
#include "extension/missing-extension-preference-view.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include "extension/manager/extension-manager.hpp"
#include "services/toast/toast-service.hpp"
#include "ui/alert/alert.hpp"
#include "ui/views/base-view.hpp"
#include "utils/environment.hpp"
#include <chrono>
#include <qlogging.h>
#include <qwidget.h>
#include <QProcessEnvironment>

NavigationController::NavigationController(ApplicationContext &ctx) : m_ctx(ctx) {}

void NavigationController::setNavigationTitle(const QString &navigationTitle, const BaseView *caller) {
  if (auto state = findViewState(VALUE_OR(caller, topView()))) {
    state->navigation.title = navigationTitle;

    if (state->sender == topView()) {
      emit navigationStatusChanged(state->navigation.title, state->navigation.icon);
    }
  }
}

bool NavigationController::hasCompleter() const {
  if (auto state = topState()) { return state->completer.has_value(); }
  return false;
}

void NavigationController::setInstantDismiss(bool value) { m_instantDismiss = value; }

void NavigationController::goBack(const GoBackOptions &opts) {
  if (!opts.ignoreInstantDismiss && m_instantDismiss) return closeWindow();

  if (isRootSearch()) {
    if (searchText().isEmpty()) return closeWindow();
    return clearSearchText();
  }

  popCurrentView();
}

void NavigationController::setSearchText(const QString &text, const BaseView *caller) {
  if (auto state = findViewState(VALUE_OR(caller, topView()))) {
    if (state->searchText == text) return;
    state->searchText = text;
    state->sender->textChanged(text);
    if (state->sender == topView()) { emit searchTextChanged(state->searchText); }
  }
}

void NavigationController::setSearchPlaceholderText(const QString &text, const BaseView *caller) {
  if (auto state = findViewState(VALUE_OR(caller, topView()))) {
    state->placeholderText = text;
    if (state->sender == topView()) { emit searchPlaceholderTextChanged(state->placeholderText); }
  }
}

void NavigationController::setLoading(bool value, const BaseView *caller) {
  if (auto state = findViewState(VALUE_OR(caller, topView()))) {
    state->isLoading = value;
    if (state->sender == topView()) { emit loadingChanged(value); }
  }
}

void NavigationController::showHud(const QString &title, const std::optional<ImageURL> &icon) {
  closeWindow();

  if (Environment::isHudDisabled()) return;

  emit showHudRequested(title, icon);
}

void NavigationController::applyPopToRoot(const PendingPopToRoot &settings) {
  auto resolveApplicablePopToRoot = [&]() {
    if (settings.type == PopToRootType::Default)
      return m_popToRootOnClose ? PopToRootType::Immediate : PopToRootType::Suspended;
    return settings.type;
  };

  PopToRootType popToRootType = resolveApplicablePopToRoot();

  switch (popToRootType) {
  case PopToRootType::Immediate:
    popToRoot({.clearSearch = true});
    break;
  default:
    break;
  }

  if (isRootSearch() && settings.clearSearch) clearSearchText();
}

void NavigationController::setDialog(DialogContentWidget *widget) { emit confirmAlertRequested(widget); }

void NavigationController::confirmAlert(const QString &title, const QString &description,
                                        const std::function<void()> &onConfirm) {
  auto widget = new CallbackAlertWidget;

  widget->setTitle(title);
  widget->setMessage(description);
  widget->setConfirmCallback(onConfirm);
  setDialog(widget);
}

void NavigationController::clearSearchText() { setSearchText(""); }

NavigationController::ViewState::~ViewState() { sender->deleteLater(); }

void NavigationController::openActionPanel() {
  if (m_isPanelOpened) return;

  m_isPanelOpened = true;
  emit actionPanelVisibilityChanged(m_isPanelOpened);
}

void NavigationController::closeActionPanel() {
  if (!m_isPanelOpened) return;

  m_isPanelOpened = false;
  emit actionPanelVisibilityChanged(m_isPanelOpened);
}

void NavigationController::toggleActionPanel() {
  m_isPanelOpened = !m_isPanelOpened;
  emit actionPanelVisibilityChanged(m_isPanelOpened);
}

void NavigationController::createCompletion(const ArgumentList &args, const ImageURL &icon) {
  if (auto state = topState()) {
    CompleterState completer(args, icon);

    state->completer = completer;
    emit completionCreated(state->completer.value_or(completer));
  }
}

void NavigationController::destroyCurrentCompletion() {
  if (auto state = topState()) {
    state->completer.reset();
    emit completionDestroyed();
  }
}

ArgumentValues NavigationController::completionValues() const {
  if (auto state = topState()) {
    if (auto completer = state->completer) { return completer->values; }

    return {};
  }

  return {};
}

void NavigationController::setCompletionValues(const ArgumentValues &values) {
  if (auto state = topState()) {
    if (state->completer) {
      state->completer->values = values;
      state->sender->argumentValuesChanged(values);
      emit completionValuesChanged(values);
    }
  }
}

void NavigationController::setNavigationIcon(const ImageURL &icon) {
  if (auto state = topState()) {
    state->navigation.icon = icon;

    if (state->sender == topView()) {
      emit navigationStatusChanged(state->navigation.title, state->navigation.icon);
    }
  }
}

void NavigationController::setNavigationSuffixIcon(const std::optional<ImageURL> &icon) {
  if (auto state = topState()) {
    if (state->sender == topView()) { emit navigationSuffixIconChanged(icon); }
  }
}

void NavigationController::popCurrentView() {
  if (isRootSearch()) return;

  auto &state = m_views.back();

  state->sender->beforePop();
  emit viewPoped(state->sender);

  m_views.pop_back();

  // always turn off instant dismiss if we are back to the root search
  // for any reason. This typically happens when `goToBack` is called with
  // `ignoreInstantDismiss` set to true and we pop back to the root.
  // This is the default behaviour of hitting backspace in an empty search bar.
  if (isRootSearch()) { m_instantDismiss = false; }

  auto &next = m_views.back();

  if (auto &accessory = next->searchAccessory) {
    emit searchAccessoryChanged(accessory.get());
  } else {
    emit searchAccessoryCleared();
  }

  emit currentViewChanged(*next.get());

  emit searchTextChanged(next->searchText);
  emit searchPlaceholderTextChanged(next->placeholderText);
  emit navigationStatusChanged(next->navigation.title, next->navigation.icon);
  emit headerVisiblityChanged(next->needsTopBar);
  emit searchVisibilityChanged(next->supportsSearch);
  emit statusBarVisiblityChanged(next->needsStatusBar);
  emit searchAccessoryVisiblityChanged(next->accessoryVisibility);
  emit loadingChanged(next->isLoading);

  if (auto cmpl = next->completer) {
    createCompletion(cmpl->args, cmpl->icon);
    setCompletionValues(cmpl->values);
  } else {
    destroyCurrentCompletion();
  }

  if (auto &ac = next->actionPanelState) emit actionsChanged(*ac);

  selectSearchText();

  if (m_frames.empty()) return;

  auto &frame = m_frames.back();

  frame->viewCount -= 1;

  if (frame->viewCount == 0) { m_frames.pop_back(); }
}

void NavigationController::popToRoot(const PopToRootOptions &opts) {
  m_pendingPopToRoot.reset();
  if (!m_frames.empty() && m_frames.back()->viewCount == 0) { m_frames.pop_back(); }

  while (m_views.size() > 1) {
    popCurrentView();
  }

  if (opts.clearSearch) clearSearchText();
}

void NavigationController::clearSearchAccessory(const BaseView *caller) {
  if (auto state = findViewState(VALUE_OR(caller, topView()))) {
    state->searchAccessory.reset();
    if (state->sender == topView()) { emit searchAccessoryCleared(); }
  }
}

void NavigationController::setSearchAccessoryVisibility(bool value, const BaseView *caller) {
  if (auto state = findViewState(VALUE_OR(caller, topView()))) {
    state->accessoryVisibility = value;
    if (state->sender == topView()) { emit searchAccessoryVisiblityChanged(value); }
  }
}

void NavigationController::selectSearchText() const { emit searchTextSelected(); }

QString NavigationController::searchText(const BaseView *caller) const {
  if (auto state = findViewState(VALUE_OR(caller, topView()))) { return state->searchText; }

  return QString();
}

bool NavigationController::isLoading(const BaseView *caller) const {
  if (auto state = findViewState(VALUE_OR(caller, topView()))) { return state->isLoading; }

  return false;
}

void NavigationController::clearActions(const BaseView *caller) {
  setActions(std::make_unique<ActionPanelState>(), caller);
}

QString NavigationController::navigationTitle(const BaseView *caller) const {
  if (auto state = findViewState(VALUE_OR(caller, topView()))) { return state->navigation.title; }

  return QString();
}

void NavigationController::setPopToRootOnClose(bool value) { m_popToRootOnClose = value; }

void NavigationController::closeWindow(const CloseWindowOptions &settings, std::chrono::milliseconds delay) {
  QTimer::singleShot(delay, [this, settings]() { return closeWindow(settings); });
}

void NavigationController::closeWindow(const CloseWindowOptions &settings) {
  if (!m_windowOpened) return;

  PopToRootType type = settings.popToRootType;

  // never pop if an overlay is shown
  if (m_ctx.overlay->hasOverlay()) { type = PopToRootType::Suspended; }

  if (m_instantDismiss) {
    qDebug() << "Consumed instantDismiss flag";
    type = PopToRootType::Immediate;
    m_instantDismiss = false;
  }

  m_pendingPopToRoot = PendingPopToRoot{.type = type, .clearSearch = settings.clearRootSearch};
  m_windowOpened = false;
  emit windowVisiblityChanged(false);
}

bool NavigationController::windowActivated() { return m_windowActivated; }

void NavigationController::setCloseOnFocusLoss(bool value) { m_closeOnFocusLoss = value; }

void NavigationController::setWindowActivated(bool value) {
  if (m_windowActivated == value) return;

  if (!value && m_closeOnFocusLoss) { closeWindow(); }

  m_windowActivated = value;
  emit windowActivationChanged(value);
}

void NavigationController::toggleWindow() {
  if (m_windowOpened) {
    if (m_windowActivated) {
      closeWindow();
      return;
    }

    // if window was not activated, we reactivate it by closing then showing (most reliable way to ensure
    // reactivation)
    closeWindow();
  }

  showWindow();
}

bool NavigationController::isWindowOpened() const { return m_windowOpened; }

void searchPlaceholderText(const QString &text) {}

bool NavigationController::executePrimaryAction() {
  auto state = topState();

  if (!state) return false;

  auto &panel = state->actionPanelState;

  if (!panel) return false;

  auto action = panel->primaryAction();

  if (!action) return false;

  executeAction(action);

  return false;
}

void NavigationController::setHeaderVisiblity(bool value, const BaseView *caller) {
  if (auto state = findViewState(VALUE_OR(caller, topView()))) {
    state->needsTopBar = value;
    if (state->sender == topView()) { emit headerVisiblityChanged(value); }
  }
}

void NavigationController::setSearchVisibility(bool value, const BaseView *caller) {
  if (auto state = findViewState(VALUE_OR(caller, topView()))) {
    state->supportsSearch = value;
    if (state->sender == topView()) { emit searchVisibilityChanged(value); }
  }
}

void NavigationController::setStatusBarVisibility(bool value, const BaseView *caller) {
  if (auto state = findViewState(VALUE_OR(caller, topView()))) {
    state->needsStatusBar = value;
    if (state->sender == topView()) { emit statusBarVisiblityChanged(value); }
  }
}

void NavigationController::executeAction(AbstractAction *action) {
  auto state = topState();

  if (!state) return;

  if (auto cmpl = state->completer; cmpl && action->isPrimary()) {
    for (int i = 0; i != cmpl->args.size() && i != cmpl->values.size(); ++i) {
      const auto &arg = cmpl->args[i];
      const auto &value = cmpl->values[i];

      if (arg.required && value.second.isEmpty()) {
        emit invalidCompletionFired();
        return;
      }
    }
  }

  action->execute(&m_ctx);
  closeActionPanel();

  if (action->autoClose()) { closeWindow({.clearRootSearch = true}); }
}

AbstractAction *NavigationController::findBoundAction(const QKeyEvent *event) const {
  auto state = topState();

  if (!state) return nullptr;
  if (!state->actionPanelState) return nullptr;

  for (const auto &section : state->actionPanelState->sections()) {
    for (const auto &action : section->actions()) {
      if (action->isBoundTo(event)) { return action.get(); }
    }
  }

  return nullptr;
}

void NavigationController::activateView(const ViewState &state) {
  if (auto &accessory = state.searchAccessory) {
    emit searchAccessoryChanged(accessory.get());
  } else {
    emit searchAccessoryCleared();
  }

  emit headerVisiblityChanged(state.needsTopBar);
  emit searchVisibilityChanged(state.supportsSearch);
  emit statusBarVisiblityChanged(state.needsStatusBar);
  emit loadingChanged(state.isLoading);
  emit navigationStatusChanged(state.navigation.title, state.navigation.icon);
  emit actionsChanged({});
  emit searchTextChanged(state.searchText);
  emit searchPlaceholderTextChanged(state.placeholderText);
  destroyCurrentCompletion();

  state.sender->initialize();
  state.sender->activate();

  emit currentViewChanged(state);
}

void NavigationController::replaceView(BaseView *view) {
  QTimer::singleShot(0, this, [this, view]() {
    m_views.back() = createViewState(view);
    activateView(*topState());
  });
}

void NavigationController::pushView(BaseView *view) {
  m_frames.back()->viewCount++;
  m_views.emplace_back(createViewState(view));
  activateView(*topState());
  emit viewPushed(view);
}

void NavigationController::setSearchAccessory(QWidget *accessory, const BaseView *caller) {
  if (auto state = findViewState(VALUE_OR(caller, topView()))) {
    state->searchAccessory.reset(accessory);

    if (state->sender == topView()) { emit searchAccessoryChanged(accessory); }
  }
}

void NavigationController::setActions(std::unique_ptr<ActionPanelState> panel, const BaseView *caller) {
  if (!panel) {
    qWarning() << "setActions called with a null pointer";
    return;
  }

  // Important: apply default shortcuts, select primary action...
  panel->finalize();

  if (auto state = findViewState(VALUE_OR(caller, topView()))) {
    state->actionPanelState = std::move(panel);
    if (state->sender == topView()) { emit actionsChanged(*state->actionPanelState.get()); }
  }
}

size_t NavigationController::viewStackSize() const { return m_views.size(); }

void NavigationController::showWindow() {
  if (auto popToRoot = m_pendingPopToRoot) {
    applyPopToRoot(*popToRoot);
    m_pendingPopToRoot.reset();
  }
  m_windowOpened = true;
  emit windowVisiblityChanged(true);
}

NavigationController::ViewState *NavigationController::topState() {
  if (m_views.empty()) return nullptr;

  return m_views.back().get();
}

const NavigationController::ViewState *NavigationController::topState() const {
  if (m_views.empty()) return nullptr;

  return m_views.back().get();
}

NavigationController::ViewState *NavigationController::findViewState(const BaseView *view) {
  return const_cast<ViewState *>(std::as_const(*this).findViewState(view));
}

bool NavigationController::isRootSearch() const { return m_views.size() == 1; }

const NavigationController::ViewState *NavigationController::findViewState(const BaseView *view) const {
  auto pred = [&](auto &&state) { return state->sender == view; };

  if (auto it = std::ranges::find_if(m_views, pred); it != m_views.end()) { return it->get(); }

  return nullptr;
}

const BaseView *NavigationController::topView() const {
  if (m_views.empty()) return nullptr;

  return m_views.back()->sender;
}

bool NavigationController::reloadActiveCommand() {
  auto cmd = activeCommand();

  if (!cmd) return false;

  // we only store the id, as the pointer will likely become invalid
  // after unloading.
  QString id = cmd->uniqueId();

  unloadActiveCommand();
  launch(id);

  return true;
}

const AbstractCmd *NavigationController::activeCommand() const {
  if (m_frames.empty()) return nullptr;

  return m_frames.back()->command.get();
}

void NavigationController::unloadActiveCommand() {
  if (m_frames.empty()) {
    qWarning() << "unloadActiveCommand called while no commands are loaded";
    return;
  }

  auto size = m_frames.back()->viewCount;

  for (int i = 0; i < size; ++i) {
    popCurrentView();
  }
}

void NavigationController::launch(const QString &id) {
  auto root = m_ctx.services->rootItemManager();

  for (ExtensionRootProvider *extension : root->extensions()) {
    for (const auto &cmd : extension->repository()->commands()) {
      if (cmd->uniqueId() == id) {
        launch(cmd);
        return;
      }
    }
  }
}

void NavigationController::launch(const std::shared_ptr<AbstractCmd> &cmd) {
  launch(cmd, completionValues());
}

void NavigationController::launch(const std::shared_ptr<AbstractCmd> &cmd, const ArgumentValues &arguments) {
  // unload stalled no-view command
  if (!m_frames.empty() && m_frames.back()->viewCount == 0) { m_frames.pop_back(); }

  if (cmd->type() == CommandType::CommandTypeExtension && !m_ctx.services->extensionManager()->isRunning()) {
    m_ctx.services->toastService()->failure("Extension manager is not running");
    return;
  }

  bool shouldCheckPreferences = cmd->type() == CommandType::CommandTypeExtension;
  LaunchProps props;

  props.arguments = arguments;

  if (shouldCheckPreferences) {
    auto itemId = QString("extension.%1").arg(cmd->uniqueId());
    auto manager = m_ctx.services->rootItemManager();
    auto preferences = manager->getMergedItemPreferences(itemId);
    auto preferenceValues = manager->getPreferenceValues(itemId);

    for (const auto &preference : preferences) {
      QJsonValue value = preferenceValues.value(preference.name());
      bool hasValue = !(value.isUndefined() || value.isNull());
      bool hasDefault = !preference.defaultValue().isUndefined();
      bool isMissing = preference.required() && !hasValue && !hasDefault;

      if (!isMissing) continue;

      if (cmd->type() == CommandType::CommandTypeExtension) {
        auto extensionCommand = std::static_pointer_cast<ExtensionCommand>(cmd);

        m_ctx.navigation->pushView(
            new MissingExtensionPreferenceView(extensionCommand, preferences, preferenceValues));
        m_ctx.navigation->setNavigationTitle(cmd->name());
        m_ctx.navigation->setNavigationIcon(cmd->iconUrl());
        return;
      }

      qDebug() << "MISSING PREFERENCE" << preference.title();
    }
  }

  auto frame = std::make_unique<CommandFrame>();

  frame->controller = std::make_unique<CommandController>(m_ctx, *cmd, props);
  frame->context.reset(cmd->createContext(cmd));
  frame->command = cmd;
  frame->viewCount = 0;
  frame->context->setContext(&m_ctx);
  m_frames.emplace_back(std::move(frame));
  m_frames.back()->context->load(props);
}

std::unique_ptr<NavigationController::ViewState> NavigationController::createViewState(BaseView *view) const {
  auto state = std::make_unique<ViewState>();
  state->sender = view;
  state->sender->setContext(&m_ctx);
  state->sender->setCommandController(m_frames.back()->controller.get());
  state->supportsSearch = view->supportsSearch();
  state->needsTopBar = view->needsGlobalTopBar();
  state->needsStatusBar = view->needsGlobalStatusBar();
  state->placeholderText = view->initialSearchPlaceholderText();
  state->navigation.title = view->initialNavigationTitle();
  state->navigation.icon = view->initialNavigationIcon();
  state->searchAccessory.reset(view->searchBarAccessory());
  return state;
}
