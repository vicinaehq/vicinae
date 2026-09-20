#pragma once
#include <qcoreapplication.h>
#include "builtins/ai/ai.hpp"
#include "command/command-database.hpp"
#include "builtins/ai/quick-ai-preferences.hpp"
#include "command/single-view-command-context.hpp"
#include "command/typed-command.hpp"
#include "services/ai/ai-preferences.hpp"
#include "services/builtin-icon/builtin-icon.hpp"
#include "theme/colors.hpp"
#include "ui/image/url.hpp"
#include "vicinae.hpp"

class QuickAICommand
    : public TypedCallbackCommand<QuickAIPreferences, NoPreferences, GuardedBuiltinCallbackCommand> {
  Q_DECLARE_TR_FUNCTIONS(QuickAICommand)

  QString id() const override { return QuickAI::qs(QuickAI::COMMAND_ID); }
  QString name() const override { return tr("Quick AI"); }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::Stars).setBackgroundTint(Omnicast::ACCENT_COLOR);
  }
  std::vector<QString> keywords() const override { return {"ask", "chat"}; }
  bool isFallback() const override { return true; }
  void execute(const Controller &controller) const override;
};

class AiExtension : public TypedCommandRepository<AiPreferences> {
  Q_DECLARE_TR_FUNCTIONS(AiExtension)

  QString id() const override { return QuickAI::qs(QuickAI::REPOSITORY_ID); }
  QString displayName() const override { return tr("AI"); }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::Atom).setBackgroundTint(SemanticColor::Purple);
  }

public:
  AiExtension() { registerCommand<QuickAICommand>(); }
};
