#include "command-actions.hpp"
#include "command-controller.hpp"

void OpenBuiltinCommandAction::execute(ApplicationContext *context) {
  QString searchText = context->navigation->searchText();

  context->command->launch(cmd);

  if (m_forwardSearchText) { context->navigation->setSearchText(searchText); }
}
