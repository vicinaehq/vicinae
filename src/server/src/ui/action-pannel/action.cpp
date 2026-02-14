#include "ui/action-pannel/action.hpp"
#include "navigation-controller.hpp"

std::unique_ptr<ActionPanelState> SubmenuAction::createSubmenuState() const {
  if (m_onOpen) { m_onOpen(); }
  return createSubmenuStateStealthily();
}

std::unique_ptr<ActionPanelState> SubmenuAction::createSubmenuStateStealthily() const {
  if (m_stateFactory) { return m_stateFactory(); }
  return nullptr;
}
