#pragma once
#include "../../ui/image/url.hpp"
#include "single-view-command-context.hpp"
#include "theme.hpp"
#include "switch-windows-view.hpp"

class SwitchWindowsCommand : public BuiltinViewCommand<SwitchWindowsView> {
  QString id() const override { return "switch-windows"; }
  QString name() const override { return "Switch Windows"; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("app-window-list").setBackgroundTint(SemanticColor::Blue);
  }
};
