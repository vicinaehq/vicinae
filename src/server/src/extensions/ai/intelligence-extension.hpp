#pragma once
#include "builtin_icon.hpp"
#include "command-database.hpp"
#include "single-view-command-context.hpp"
#include "transcribe-view-host.hpp"
#include "ui/image/url.hpp"

class TranscribeCommand : public BuiltinViewCommand<TranscribeViewHost> {
  QString id() const override { return "transcribe"; }
  QString name() const override { return "Transcribe"; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::Microphone).setBackgroundTint(Omnicast::ACCENT_COLOR);
  }
  std::vector<QString> keywords() const override { return {"dictate"}; }
};

class IntelligenceExtension : public BuiltinCommandRepository {
  QString id() const override { return "intelligence"; }
  QString displayName() const override { return "Intelligence"; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::Stars).setBackgroundTint(Omnicast::ACCENT_COLOR);
  }

public:
  IntelligenceExtension() { registerCommand<TranscribeCommand>(); }
};
