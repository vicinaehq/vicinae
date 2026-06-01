#pragma once
#include <memory>
#include <QtGlobal>
#include "services/global-shortcuts/abstract-global-shortcut-backend.hpp"

#ifdef Q_OS_LINUX
class LinuxInputServer;

/**
 * Builds the highest-priority activatable backend for the current environment, falling back to a
 * dummy when none is available.
 */
std::unique_ptr<AbstractGlobalShortcutBackend> createGlobalShortcutBackend(LinuxInputServer *inputServer);
#else
std::unique_ptr<AbstractGlobalShortcutBackend> createGlobalShortcutBackend();
#endif
