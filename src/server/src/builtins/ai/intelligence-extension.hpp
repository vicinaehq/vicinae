#pragma once
#include "services/builtin-icon/builtin-icon.hpp"
#include "command/command-database.hpp"
#include "theme/colors.hpp"
#include "ui/image/url.hpp"
#include <qcoreapplication.h>

// probably going to wrap generic AI chat commands, not sure.
// Dictation has its own extension, as it's big enough in itself
class IntelligenceExtension : public BuiltinCommandRepository {
  Q_DECLARE_TR_FUNCTIONS(IntelligenceExtension)

  QString id() const override { return "intelligence"; }
  QString displayName() const override { return tr("Intelligence"); }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::Stars).setBackgroundTint(Omnicast::ACCENT_COLOR);
  }

public:
  IntelligenceExtension() = default;
};
