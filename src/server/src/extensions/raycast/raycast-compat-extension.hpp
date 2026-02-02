#pragma once
#include "ui/image/url.hpp"
#include "raycast-store-command.hpp"

class RaycastCompatExtension : public BuiltinCommandRepository {
public:
  QString id() const override { return "raycast-compat"; }
  QString displayName() const override { return "Raycast Compat"; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("raycast").setBackgroundTint(SemanticColor::Red);
  }
  QString description() const override { return "Raycast compatiblity features"; }

  RaycastCompatExtension() { registerCommand<RaycastStoreCommand>(); }
};
