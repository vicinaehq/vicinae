#include "command-database.hpp"
#include "ui/image/url.hpp"
#include "vicinae.hpp"
#include <QCoreApplication>
#include <qurlquery.h>

class VicinaeExtension : public BuiltinCommandRepository {
  QString id() const override { return "core"; }
  QString displayName() const override { return "Vicinae"; }
  QString description() const override {
    return QCoreApplication::translate("VicinaeExtension", "General vicinae-related commands.");
  }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("vicinae").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }

public:
  VicinaeExtension();
};
