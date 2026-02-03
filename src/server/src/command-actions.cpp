#include "command-actions.hpp"
#include "navigation-controller.hpp"

void OpenBuiltinCommandAction::execute(ApplicationContext *context) {
  QString searchText = context->navigation->searchText();

  context->navigation->launch(cmd);

  if (m_forwardSearchText) { context->navigation->setSearchText(searchText); }
}
