#pragma once
#include "command-database.hpp"
#include "../../ui/image/url.hpp"
#include "manage-models-command.hpp"

class IntelligenceExtension : public BuiltinCommandRepository {
  QString id() const override { return "intelligence"; }
  QString displayName() const override { return "Intelligence"; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("stars").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }

public:
  IntelligenceExtension() { registerCommand<ManageModelsCommand>(); }
};
