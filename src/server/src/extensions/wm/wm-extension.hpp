#pragma once
#include "command-database.hpp"
#include "../../ui/image/url.hpp"
#include "theme.hpp"
#include <qcontainerfwd.h>
#include "switch-windows-command.hpp"

class WindowManagementExtension : public BuiltinCommandRepository {
  QString id() const override { return "wm"; }
  QString displayName() const override { return "Window Management"; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("app-window-list").setBackgroundTint(SemanticColor::Blue);
  }

public:
  WindowManagementExtension() { registerCommand<SwitchWindowsCommand>(); }
};
