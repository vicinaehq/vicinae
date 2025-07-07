#include "calculator-actions.hpp"

void OpenCalculatorHistoryAction::execute() {
  auto ui = ServiceRegistry::instance()->UI();

  ui->launchCommand("calculator.history");
}
