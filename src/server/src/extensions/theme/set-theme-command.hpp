#pragma once
#include "qml/bridge-view.hpp"
#include "qml/theme-list-model.hpp"
#include "single-view-command-context.hpp"
#include "ui/image/url.hpp"

class SetThemeCommand : public BuiltinViewCommand<BridgeView<ThemeListModel>> {
  QString id() const override { return "set"; }
  QString name() const override { return "Set Theme"; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("brush").setBackgroundTint(SemanticColor::Purple);
  }
};
