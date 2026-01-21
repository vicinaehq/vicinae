#pragma once
#include "builtin_icon.hpp"
#include "command-database.hpp"
#include "theme/colors.hpp"
#include "ui/image/url.hpp"

class BrowserExtension : public BuiltinCommandRepository {
  QString id() const override { return "browser-extension"; }
  QString displayName() const override { return "Browser Extension"; }
  QString description() const override { return "Browser extension related commands."; }
  ImageURL iconUrl() const override {
    return ImageURL(BuiltinIcon::Globe01).setBackgroundTint(SemanticColor::Red);
  }

public:
  BrowserExtension();
};
