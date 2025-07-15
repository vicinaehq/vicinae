#include "navigation-controller.hpp"
#include "base-view.hpp"

void NavigationController::setNavigationTitle(const QString &navigationTitle, const BaseView *caller) {
  if (auto state = findViewState(VALUE_OR(caller, topView()))) {
    state->navigation.title = navigationTitle;

    if (caller == topView()) { emit currentViewStateChanged(*state); }
  }
}

void NavigationController::setSearchText(const QString &text, const BaseView *caller) {
  if (auto state = findViewState(VALUE_OR(caller, topView()))) {
    state->searchText = text;
    state->sender->textChanged(text);

    if (caller == topView()) { emit currentViewStateChanged(*state); }
  }
}

void NavigationController::setSearchPlaceholderText(const QString &text) {
  if (auto state = topState()) {
    state->placeholderText = text;
    emit currentViewStateChanged(*state);
  }
}

void NavigationController::clearSearchText() { setSearchText(""); }

NavigationController::ViewState::~ViewState() { sender->deleteLater(); }

void NavigationController::setNavigationIcon(const OmniIconUrl &icon) {
  if (auto state = topState()) {
    state->navigation.icon = icon;
    emit currentViewStateChanged(*state);
  }
}

void NavigationController::popCurrentView() {
  if (m_views.size() < 2) return;

  auto &state = m_views.back();

  emit viewPoped(state->sender);
  m_views.pop_back();

  auto &next = m_views.back();

  emit currentViewChanged(*next.get());
  emit currentViewStateChanged(*next.get());
}

QString NavigationController::searchText(const BaseView *caller) const {
  if (auto state = findViewState(VALUE_OR(caller, topView()))) { return state->searchText; }

  return QString();
}

void searchPlaceholderText(const QString &text) {}

void NavigationController::pushView(BaseView *view) {
  auto state = std::make_unique<ViewState>();

  state->sender = view;
  state->supportsSearch = view->supportsSearch();
  state->needsTopBar = view->needsGlobalTopBar();
  state->supportsSearch = view->needsGlobalStatusBar();

  state->sender->initialize();
  state->sender->activate();

  emit currentViewChanged(*state.get());
  m_views.emplace_back(std::move(state));
  emit viewPushed(view);
}

size_t NavigationController::viewStackSize() const { return m_views.size(); }

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
