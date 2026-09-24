#pragma once
#include <qcoreapplication.h>
#include "builtins/ai/ai.hpp"
#include "conversation-history-view-host.hpp"
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

class AIChatCommand : public BuiltinCallbackCommand {
  Q_DECLARE_TR_FUNCTIONS(AIChatCommand)
  QString id() const override { return QStringLiteral("chat"); }
  QString name() const override { return tr("AI Chat"); }
  QString description() const override { return tr("Open your AI conversations in a dedicated window."); }
  std::vector<QString> keywords() const override { return {"chat", "conversation", "window"}; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::Stars).setBackgroundTint(Omnicast::ACCENT_COLOR);
  }
  void execute(CommandController &controller) const override;
};

class ConversationHistoryCommand : public BuiltinViewCommand<ConversationHistoryViewHost> {
  Q_DECLARE_TR_FUNCTIONS(ConversationHistoryCommand)
  QString id() const override { return QStringLiteral("conversations"); }
  QString name() const override { return tr("Browse AI Conversations"); }
  std::vector<QString> keywords() const override { return {"chat", "history"}; }
  QString description() const override { return tr("Reopen and continue previous AI conversations."); }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::Stars).setBackgroundTint(Omnicast::ACCENT_COLOR);
  }
};

class AiExtension : public TypedCommandRepository<AiPreferences> {
  Q_DECLARE_TR_FUNCTIONS(AiExtension)

  QString id() const override { return QuickAI::qs(QuickAI::REPOSITORY_ID); }
  QString displayName() const override { return tr("AI"); }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::Atom).setBackgroundTint(SemanticColor::Purple);
  }
  std::vector<SettingsPage> settingsPages() const override {
    return {{.id = "tools",
             .title = tr("Tools"),
             .description = tr("Choose which tools AI models can use across your extensions."),
             .component = QStringLiteral("AIToolsSettingsPage")}};
  }
  void preferencesChanged(const AiPreferences &preferences) const override;

public:
  AiExtension() {
    registerCommand<QuickAICommand>();
    registerCommand<AIChatCommand>();
    registerCommand<ConversationHistoryCommand>();
  }
};
