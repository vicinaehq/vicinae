#include "calculator-actions.hpp"
#include "common.hpp"

void OpenCalculatorHistoryAction::execute(ApplicationContext *ctx) {
  ctx->navigation->launch("calculator.history");
}
