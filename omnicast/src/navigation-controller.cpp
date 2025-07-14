#include "navigation-controller.hpp"

void NavigationController::setNavigationTitle(const QString &navigationTitle) {
  if (auto state = topState()) {
    state->navigation.title = navigationTitle;
    emit currentViewStateChanged(*state);
  }
}

void NavigationController::setSearchText(const QString &text) {
  if (auto state = topState()) {
    state->text = text;
    emit currentViewStateChanged(*state);
  }
}

void NavigationController::setSearchPlaceholderText(const QString &text) {
  if (auto state = topState()) {
    state->placeholderText = text;
    emit currentViewStateChanged(*state);
  }
}

void NavigationController::setNavigationIcon(const OmniIconUrl &icon) {
  if (auto state = topState()) {
    state->navigation.icon = icon;
    emit currentViewStateChanged(*state);
  }
}

NavigationController::ViewState *NavigationController::topState() {
  if (m_commandFrames.empty()) return nullptr;

  for (auto it = m_commandFrames.rbegin(); it != m_commandFrames.rend(); ++it) {
    if (!(*it)->viewStack.empty()) return &(*it)->viewStack.back();
  }

  return nullptr;
}

const NavigationController::ViewState *NavigationController::topState() const {
  if (m_commandFrames.empty()) return nullptr;

  for (auto it = m_commandFrames.rbegin(); it != m_commandFrames.rend(); ++it) {
    if (!(*it)->viewStack.empty()) return &(*it)->viewStack.back();
  }

  return nullptr;
}
