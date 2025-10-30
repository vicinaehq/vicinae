#pragma once
#include "single-view-command-context.hpp"
#include "store/store-listing-view.hpp"
#include "theme.hpp"
#include "vicinae.hpp"

class VicinaeStoreCommand : public BuiltinViewCommand<VicinaeStoreListingView> {
  QString id() const override { return "store"; }
  QString name() const override { return "Extension Store"; }
  QString description() const override { return "Install extensions from the Vicinae store"; }
  QString extensionId() const override { return "vicinae"; }
  QString commandId() const override { return "store"; }
  ImageURL iconUrl() const override {
    auto icon = ImageURL::builtin("cart");
    icon.setBackgroundTint(Omnicast::ACCENT_COLOR);
    return icon;
  }
  std::vector<Preference> preferences() const override { return {}; }
};
