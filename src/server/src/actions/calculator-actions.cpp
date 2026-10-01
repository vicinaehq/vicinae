#include "calculator-actions.hpp"

void OpenCalculatorHistoryAction::execute(ApplicationContext *ctx) {
  ctx->navigation->activateEntrypoint(EntrypointId{"calculator", "history"});
}
