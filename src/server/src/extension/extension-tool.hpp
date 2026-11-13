#pragma once
#include <QCoreApplication>
#include <memory>
#include "extension/extension-command.hpp"
#include "services/ai/ai-tool.hpp"
#include "services/extension-registry/extension-manifest.hpp"

// tools are not root items, their preferences are stored separately
class ExtensionToolCommand : public ExtensionCommand {
public:
  using ExtensionCommand::ExtensionCommand;

  PreferenceValues preferenceValues(const RootItemManager &rootItemManager) const override;
  void openPreferences(SettingsController &settings) const override;
};

class ExtensionTool : public AI::AbstractTool {
  Q_DECLARE_TR_FUNCTIONS(ExtensionTool)

public:
  ExtensionTool(ExtensionManifest::Tool tool, std::shared_ptr<ExtensionCommand> command)
      : m_tool(std::move(tool)), m_command(std::move(command)) {}

  std::string name() const override { return m_tool.name; }
  std::string description() const override;
  std::string generateInputSchema() const override;
  bool requiresApproval(std::string_view) const override { return m_tool.confirmation; }
  RawToolTask runRaw(std::string_view arguments) override;

private:
  ExtensionManifest::Tool m_tool;
  std::shared_ptr<ExtensionCommand> m_command;
};
