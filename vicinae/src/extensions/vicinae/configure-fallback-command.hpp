#pragma once
#include "manage-fallback-commands.hpp"
#include "../../ui/image/url.hpp"
#include "single-view-command-context.hpp"

class ManageFallbackCommand : public BuiltinViewCommand<ManageFallbackCommandsView> {
  QString id() const override { return "manage-fallback"; }
  QString name() const override { return "Configure Fallback Commands"; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("undo").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }
  QString description() const override {
    return "Configure what commands are to be presented as fallback options when nothing matches the search "
           "in the root search.";
  }
};
