#pragma once

#include "qml/installed-extensions-view-host.hpp"
#include "single-view-command-context.hpp"

class VicinaeListInstalledExtensionsCommand : public BuiltinViewCommand<InstalledExtensionsViewHost> {
  QString id() const override { return "list-extensions"; }
  QString name() const override { return "Show Installed Extensions"; }
  QString description() const override {
    return "Show all third-party extensions that have been installed. This includes local extensions as well "
           "as extensions downloaded from the stores (vicinae and raycast).";
  }
  ImageURL iconUrl() const override {
    auto icon = ImageURL::builtin("plug");
    icon.setBackgroundTint(Omnicast::ACCENT_COLOR);
    return icon;
  }
};
