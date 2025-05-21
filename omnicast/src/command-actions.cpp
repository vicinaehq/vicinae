#include "command-actions.hpp"
#include "service-registry.hpp"

void OpenBuiltinCommandAction::execute() { ServiceRegistry::instance()->UI()->launchCommand(cmd); }
