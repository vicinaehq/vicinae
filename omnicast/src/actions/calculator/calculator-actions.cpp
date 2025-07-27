#include "calculator-actions.hpp"
#include "common.hpp"
#include "command-controller.hpp"

void OpenCalculatorHistoryAction::execute(ApplicationContext *ctx) {

  ctx->command->launch("calculator.history");
}
