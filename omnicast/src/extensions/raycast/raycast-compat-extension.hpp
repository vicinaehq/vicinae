#pragma once
#include "common.hpp"
#include "omni-icon.hpp"
#include "raycast-store-command.hpp"

class RaycastCompatExtension : public AbstractCommandRepository {
public:
  QString id() const override { return "raycast-compat"; }
  QString name() const override { return "Raycast Compat"; }
  OmniIconUrl iconUrl() const override {
    return BuiltinOmniIconUrl("raycast").setBackgroundTint(SemanticColor::Red);
  }
  QString description() const override { return "Raycast compatiblity features"; }
  std::vector<std::shared_ptr<AbstractCmd>> commands() const override {
    auto store = std::make_shared<RaycastStoreCommand>();

    return {store};
  }
};
