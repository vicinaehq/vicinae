#pragma once
#include <optional>
#include <string>
#include <vector>
#include <QObject>
#include <QString>
#include "services/ai-commands/ai-command-store.hpp"
#include "services/ai/text-rewrite.hpp"
#include "services/global-shortcuts/global-shortcut-service.hpp"
#include "services/paste/paste-service.hpp"
#include "services/selection/abstract-selection-service.hpp"
#include "ui/windows/hud-activity.hpp"
#include "utils/placeholder.hpp"

struct ApplicationContext;
class ServiceRegistry;

/**
 * One run of an AI command: expands its prompt, reading the selected text when the prompt uses it, and
 * pastes the response in the active app. Deletes itself once finished.
 */
class AiCommandRun : public HudActivity {
  Q_OBJECT

public:
  void cancel() override;

  AiCommandRun(ServiceRegistry &services, const AiCommandStore::Command &command,
               std::vector<QString> arguments, PasteService::Target pasteTarget, QObject *parent = nullptr);

  void start();
  bool isFinished() const { return m_finished; }

private:
  void handleSelection(const AbstractSelectionService::Result &result);
  void send(const std::optional<QString> &selection);
  void deliver(const QString &text);
  void finish();
  void finishWithMessage(const QString &message);

  ServiceRegistry &m_services;
  std::string m_model;
  AiCommandStore::Output m_output;
  PlaceholderString m_prompt;
  std::vector<QString> m_arguments;
  PasteService::Target m_pasteTarget;
  TextRewrite m_rewrite;
  GlobalShortcutHandle m_cancelShortcut;
  bool m_finished = false;
};

namespace AiCommands {

/**
 * Runs the command with the values of its argument fields, or cancels the one already running.
 */
void run(const ApplicationContext *ctx, const AiCommandStore::Command &command,
         const std::vector<QString> &arguments);

} // namespace AiCommands
