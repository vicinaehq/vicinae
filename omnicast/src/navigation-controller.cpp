#include "navigation-controller.hpp"
#include "base-view.hpp"
#include <qlogging.h>
#include <qwidget.h>

NavigationController::NavigationController(ApplicationContext &ctx) : m_ctx(ctx) {}

void NavigationController::setNavigationTitle(const QString &navigationTitle, const BaseView *caller) {
  if (auto state = findViewState(VALUE_OR(caller, topView()))) {
    state->navigation.title = navigationTitle;

    if (state->sender == topView()) {
      emit navigationStatusChanged(state->navigation.title, state->navigation.icon);
    }
  }
}

void NavigationController::setSearchText(const QString &text, const BaseView *caller) {
  if (auto state = findViewState(VALUE_OR(caller, topView()))) {
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
    state->loading = value;
    if (state->sender == topView()) { emit loadingChanged(value); }
  }
}

void NavigationController::showHud(const QString &title, const std::optional<OmniIconUrl> &icon) {
  closeWindow();
  emit showHudRequested(title, icon);
}

void NavigationController::setDialog(DialogContentWidget *widget) { emit confirmAlertRequested(widget); }

void NavigationController::clearSearchText() { setSearchText(""); }

NavigationController::ViewState::~ViewState() { sender->deleteLater(); }

void NavigationController::openActionPanel() { emit actionPanelVisibilityChanged(true); }

void NavigationController::closeActionPanel() { emit actionPanelVisibilityChanged(false); }

void NavigationController::createCompletion(const ArgumentList &args, const OmniIconUrl &icon) {
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
    if (state->completer) { state->completer->values = values; }
  }
}

void NavigationController::setNavigationIcon(const OmniIconUrl &icon) {
  if (auto state = topState()) {
    state->navigation.icon = icon;

    if (state->sender == topView()) {
      emit navigationStatusChanged(state->navigation.title, state->navigation.icon);
    }
  }
}

void NavigationController::popCurrentView() {
  if (m_views.size() < 2) return;

  auto &state = m_views.back();

  emit viewPoped(state->sender);
  m_views.pop_back();

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
  emit loadingChanged(next->isLoading);

  if (auto &ac = next->actionPanelState) emit actionsChanged(*ac);

  selectSearchText();
}

void NavigationController::popToRoot() {
  while (m_views.size() > 1) {
    popCurrentView();
  }
}

void NavigationController::clearSearchAccessory(const BaseView *caller) {
  if (auto state = findViewState(VALUE_OR(caller, topView()))) {
    state->searchAccessory.reset();
    if (state->sender == topView()) { emit searchAccessoryCleared(); }
  }
}

void NavigationController::selectSearchText() const { emit searchTextSelected(); }

QString NavigationController::searchText(const BaseView *caller) const {
  if (auto state = findViewState(VALUE_OR(caller, topView()))) { return state->searchText; }

  return QString();
}

void NavigationController::clearActions(const BaseView *caller) {
  setActions(std::make_unique<ActionPanelState>(), caller);
}

QString NavigationController::navigationTitle(const BaseView *caller) const {
  if (auto state = findViewState(VALUE_OR(caller, topView()))) { return state->navigation.title; }

  return QString();
}

void NavigationController::toggleWindow() {
  if (m_windowOpened)
    closeWindow();
  else
    showWindow();
}

bool NavigationController::isWindowOpened() const { return m_windowOpened; }

void searchPlaceholderText(const QString &text) {}

bool NavigationController::executePrimaryAction() {
  if (auto state = topState()) {
    if (auto &panel = state->actionPanelState) {
      if (auto primary = panel->findPrimaryAction()) { executeAction(primary); }
    }
    return false;
  }

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
  action->execute(&m_ctx);
  closeActionPanel();
}

void NavigationController::pushView(BaseView *view) {
  auto state = std::make_unique<ViewState>();

  state->sender = view;
  state->supportsSearch = view->supportsSearch();
  state->needsTopBar = view->needsGlobalTopBar();
  state->needsStatusBar = view->needsGlobalStatusBar();
  state->searchAccessory.reset(view->searchBarAccessory());
  state->sender->setContext(&m_ctx);

  if (auto &accessory = state->searchAccessory) {
    emit searchAccessoryChanged(accessory.get());
  } else {
    emit searchAccessoryCleared();
  }

  emit headerVisiblityChanged(state->needsTopBar);
  emit searchVisibilityChanged(state->supportsSearch);
  emit statusBarVisiblityChanged(state->needsStatusBar);

  m_views.emplace_back(std::move(state));

  emit actionsChanged({});
  emit searchTextChanged(QString());
  emit searchPlaceholderTextChanged(QString());
  view->initialize();
  view->activate();
  emit currentViewChanged(*m_views.back());
  emit viewPushed(view);
}

void NavigationController::setSearchAccessory(QWidget *accessory, const BaseView *caller) {
  if (auto state = findViewState(VALUE_OR(caller, topView()))) {
    state->searchAccessory.reset(accessory);

    if (state->sender == topView()) { emit searchAccessoryChanged(accessory); }
  }
}

void NavigationController::setActions(std::unique_ptr<ActionPanelState> panel, const BaseView *caller) {
  qDebug() << "set actions called";
  if (!panel) {
    qCritical() << "setActions called with a null pointer";
    return;
  }

  if (auto state = findViewState(VALUE_OR(caller, topView()))) {
    state->actionPanelState = std::move(panel);
    if (state->sender == topView()) { emit actionsChanged(*state->actionPanelState.get()); }
  }
}

size_t NavigationController::viewStackSize() const { return m_views.size(); }

void NavigationController::closeWindow() {
  m_windowOpened = false;
  emit windowVisiblityChanged(false);
}

void NavigationController::showWindow() {
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

const NavigationController::ViewState *NavigationController::findViewState(const BaseView *view) const {
  auto pred = [&](auto &&state) { return state->sender == view; };

  if (auto it = std::ranges::find_if(m_views, pred); it != m_views.end()) { return it->get(); }

  return nullptr;
}

const BaseView *NavigationController::topView() const {
  if (m_views.empty()) return nullptr;

  return m_views.back()->sender;
}
