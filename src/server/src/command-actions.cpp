#include "command-actions.hpp"
#include "navigation-controller.hpp"

void OpenBuiltinCommandAction::execute(ApplicationContext *context) {
  QString const searchText = context->navigation->searchText();

  if (m_forwardSearchText && !searchText.isEmpty()) {
    context->navigation->launch(cmd, LaunchProps{.fallbackText = searchText});
    context->navigation->setSearchText(searchText);
  } else {
    context->navigation->launch(cmd);
  }
}
