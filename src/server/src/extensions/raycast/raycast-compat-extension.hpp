#pragma once
#include "ui/image/url.hpp"
#include "raycast-store-command.hpp"
#include <QCoreApplication>

class RaycastCompatExtension : public BuiltinCommandRepository {
  Q_DECLARE_TR_FUNCTIONS(RaycastCompatExtension)

public:
  QString id() const override { return "raycast-compat"; }
  QString displayName() const override { return QStringLiteral("Raycast Compat"); }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("raycast").setBackgroundTint(SemanticColor::Red);
  }
  QString description() const override { return tr("Raycast compatibility features"); }

  RaycastCompatExtension() { registerCommand<RaycastStoreCommand>(); }
};
