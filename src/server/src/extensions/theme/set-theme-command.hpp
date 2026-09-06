#pragma once
#include "extensions/theme/theme-view-host.hpp"
#include "command/single-view-command-context.hpp"
#include "ui/image/url.hpp"
#include <QCoreApplication>

class SetThemeCommand : public BuiltinViewCommand<ThemeViewHost> {
  QString id() const override { return "set"; }
  QString name() const override { return QCoreApplication::translate("SetThemeCommand", "Set Theme"); }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::Brush).setBackgroundTint(SemanticColor::Purple);
  }
};
