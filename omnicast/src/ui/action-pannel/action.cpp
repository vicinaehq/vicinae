#include "ui/action-pannel/action.hpp"
#include "app.hpp"

void AbstractAction::executePrelude(AppWindow &app) { app.actionPannel->close(); }
