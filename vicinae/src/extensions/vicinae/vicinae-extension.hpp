#include "command-database.hpp"
#include "ui/image/url.hpp"
#include "vicinae.hpp"
#include <qsqlquery.h>
#include <qurlquery.h>

class VicinaeExtension : public BuiltinCommandRepository {
  QString id() const override { return "core"; }
  QString displayName() const override { return "Vicinae"; }
  QString description() const override { return "General vicinae-related commands."; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("vicinae").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }

public:
  VicinaeExtension();
};
