#pragma once
#include "script/script-command-file.hpp"
#include "ui/action-pannel/action.hpp"

class ScriptExecutorAction : public AbstractAction {
public:
  ScriptExecutorAction(const std::shared_ptr<ScriptCommandFile> &file,
                       std::optional<script_command::OutputMode> mode = std::nullopt);

  void execute(ApplicationContext *ctx) override;
  QString title() const override;
  std::optional<ImageURL> icon() const override;

private:
  static void executeOneLine(const ScriptCommandFile &script, const std::vector<QString> &args,
                             const std::function<void(bool ok, QString line)> &callback);

  std::shared_ptr<ScriptCommandFile> m_file;
  std::optional<script_command::OutputMode> m_outputModeOverride;
};
