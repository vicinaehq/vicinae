#pragma once
#include <qcoreapplication.h>
#include "command/command-database.hpp"
#include "services/builtin-icon/builtin-icon.hpp"
#include "theme/colors.hpp"
#include "ui/image/url.hpp"

class AiExtension : public BuiltinCommandRepository {
  Q_DECLARE_TR_FUNCTIONS(AiExtension)

  QString id() const override { return "ai"; }
  QString displayName() const override { return tr("AI"); }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::Atom).setBackgroundTint(SemanticColor::Purple);
  }

public:
  AiExtension() = default;
};
