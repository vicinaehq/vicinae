#include "calculator-actions.hpp"
#include "common.hpp"

void OpenCalculatorHistoryAction::execute(ApplicationContext *ctx) {
  ctx->navigation->activateEntrypoint(EntrypointId{"calculator", "history"});
}
