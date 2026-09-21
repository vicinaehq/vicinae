#pragma once
#include <ApplicationServices/ApplicationServices.h>
#include "services/window-manager/abstract-window-manager.hpp"

namespace MacosSpaces {

bool supportsWindowMove();
bool supportsSpaceSwitch();
QFuture<AbstractWindowManager::WorkspaceChangeResult>
moveWindow(AXUIElementRef window, AbstractWindowManager::Direction direction, QObject *context);
QFuture<AbstractWindowManager::WorkspaceChangeResult> switchSpace(AbstractWindowManager::Direction direction,
                                                                  QObject *context);

} // namespace MacosSpaces
