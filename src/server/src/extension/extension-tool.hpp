#pragma once
#include <QCoreApplication>
#include <memory>
#include "services/ai/ai-tool.hpp"
#include "services/extension-registry/extension-manifest.hpp"

struct ApplicationContext;
class ExtensionCommand;

class ExtensionToolRunner {
  Q_DECLARE_TR_FUNCTIONS(ExtensionToolRunner)

public:
  explicit ExtensionToolRunner(ApplicationContext &ctx) : m_ctx(ctx) {}

  AI::AbstractTool::RawToolTask run(const std::shared_ptr<ExtensionCommand> &command,
                                    std::string_view arguments, bool requiresApproval);

private:
  ApplicationContext &m_ctx;
};

class ExtensionTool : public AI::AbstractTool {
  Q_DECLARE_TR_FUNCTIONS(ExtensionTool)

public:
  ExtensionTool(ExtensionManifest::Tool tool, std::shared_ptr<ExtensionCommand> command)
      : m_tool(std::move(tool)), m_command(std::move(command)) {}

  std::string name() const override { return m_tool.name; }
  std::string description() const override;
  std::string generateInputSchema() const override;
  bool handlesApproval() const override { return true; }
  RawToolTask runRaw(std::string_view arguments) override;

private:
  ExtensionManifest::Tool m_tool;
  std::shared_ptr<ExtensionCommand> m_command;
};
