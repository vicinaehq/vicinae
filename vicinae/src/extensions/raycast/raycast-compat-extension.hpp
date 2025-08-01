#pragma once
#include "common.hpp"
#include "omni-icon.hpp"
#include "raycast-store-command.hpp"

class RaycastCompatExtension : public BuiltinCommandRepository {
public:
  QString id() const override { return "raycast-compat"; }
  QString name() const override { return "Raycast Compat"; }
  OmniIconUrl iconUrl() const override {
    return BuiltinOmniIconUrl("raycast").setBackgroundTint(SemanticColor::Red);
  }
  QString description() const override { return "Raycast compatiblity features"; }

  RaycastCompatExtension() { registerCommand<RaycastStoreCommand>(); }
};
