#include "command-actions.hpp"
#include "service-registry.hpp"
#include "base-view.hpp"
#include "command-controller.hpp"

void OpenBuiltinCommandAction::execute(ApplicationContext *context) { context->command->launch(cmd); }

void OpenBuiltinCommandAction::execute() {
  auto ui = ServiceRegistry::instance()->UI();

  ui->launchCommand(cmd);

  if (!text.isEmpty()) { ui->topView()->setSearchText(text); }
}
