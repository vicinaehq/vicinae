#pragma once
#include "qml/switch-windows-view-host.hpp"
#include "single-view-command-context.hpp"
#include "theme.hpp"
#include <QCoreApplication>

class SwitchWindowsCommand : public BuiltinViewCommand<SwitchWindowsViewHost> {
  QString id() const override { return "switch-windows"; }
  QString name() const override {
    return QCoreApplication::translate("SwitchWindowsCommand", "Switch Windows");
  }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("app-window-list").setBackgroundTint(SemanticColor::Cyan);
  }
};
