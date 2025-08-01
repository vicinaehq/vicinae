#pragma once
#include "omni-icon.hpp"
#include "single-view-command-context.hpp"
#include "theme.hpp"
#include "switch-windows-view.hpp"

class SwitchWindowsCommand : public BuiltinViewCommand<SwitchWindowsView> {
  QString id() const override { return "switch-windows"; }
  QString name() const override { return "Switch Windows"; }
  OmniIconUrl iconUrl() const override {
    return BuiltinOmniIconUrl("app-window-list").setBackgroundTint(SemanticColor::Blue);
  }
};
