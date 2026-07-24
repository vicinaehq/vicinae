#pragma once
#include "builtin_icon.hpp"
#include "command-database.hpp"
#include "theme/colors.hpp"
#include "ui/image/url.hpp"
#include <QCoreApplication>

class BrowserExtension : public BuiltinCommandRepository {
  Q_DECLARE_TR_FUNCTIONS(BrowserExtension)

  QString id() const override { return "browser-extension"; }
  QString displayName() const override { return tr("Browser Extension"); }
  QString description() const override { return tr("Browser extension related commands."); }
  ImageURL iconUrl() const override {
    return ImageURL(BuiltinIcon::Globe01).setBackgroundTint(SemanticColor::Red);
  }

public:
  BrowserExtension();
};
