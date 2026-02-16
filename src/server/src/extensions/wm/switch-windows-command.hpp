#pragma once
#include "qml/qml-switch-windows-view-host.hpp"
#include "single-view-command-context.hpp"
#include "theme.hpp"

class SwitchWindowsCommand : public BuiltinViewCommand<QmlSwitchWindowsViewHost> {
  QString id() const override { return "switch-windows"; }
  QString name() const override { return "Switch Windows"; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("app-window-list").setBackgroundTint(SemanticColor::Blue);
  }
};
