#include "command-actions.hpp"
#include "command-controller.hpp"

void OpenBuiltinCommandAction::execute(ApplicationContext *context) { context->command->launch(cmd); }
