#pragma once
#include "browse/browse-fonts-view.hpp"
#include "single-view-command-context.hpp"

class BrowseFontsCommand : public BuiltinViewCommand<BrowseFontsView> {
  QString id() const override { return "browse"; }
  QString name() const override { return "Browse Fonts"; };
  ImageURL iconUrl() const override {
    return ImageURL::builtin("text").setBackgroundTint(SemanticColor::Orange);
  }
};
