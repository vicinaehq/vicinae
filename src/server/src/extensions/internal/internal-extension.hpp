#pragma once
#include "command-database.hpp"
#include "single-view-command-context.hpp"
#include "vicinae.hpp"
#include "playground/playground-view.hpp"

class UIPlayground : public BuiltinViewCommand<PlaygroundView> {
  QString id() const override { return "playground"; }
  QString name() const override { return "UI Playground"; }
  QString description() const override { return "Playground description"; }
  bool isFallback() const override { return false; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("opentelemetry").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }

  virtual bool isInternal() const override { return true; }
};

class InternalExtension : public BuiltinCommandRepository {
  QString id() const override { return "internal"; }
  QString displayName() const override { return "Internal Commands"; }
  QString description() const override { return "Internal Commands"; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("magnifying-glass").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }

public:
  InternalExtension() { registerCommand<UIPlayground>(); }
};
