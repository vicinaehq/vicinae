#include "calculator-actions.hpp"
#include "common.hpp"

void OpenCalculatorHistoryAction::execute(ApplicationContext *ctx) {
  ctx->navigation->launch(EntrypointId{"calculator", "history"});
}
