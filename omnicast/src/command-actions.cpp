#include "command-actions.hpp"
#include "service-registry.hpp"
#include "base-view.hpp"

void OpenBuiltinCommandAction::execute() {
  auto ui = ServiceRegistry::instance()->UI();

  ui->launchCommand(cmd);

  if (!text.isEmpty()) { ui->topView()->setSearchText(text); }
}
