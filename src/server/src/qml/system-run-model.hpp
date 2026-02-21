#pragma once
#include "common/scored.hpp"
#include "command-list-model.hpp"
#include <filesystem>
#include <variant>

using CommandLine = std::vector<std::string>;
using RunProgramItem = std::variant<CommandLine, std::filesystem::path>;

class SystemRunModel : public CommandListModel {
  Q_OBJECT

public:
  enum class DefaultAction { RunInTerminal, RunInTerminalHold, Run };

  static DefaultAction parseDefaultAction(QStringView s);

  using CommandListModel::CommandListModel;

  void setDefaultAction(DefaultAction action) { m_defaultAction = action; }

  void setData(std::vector<std::string> cmdline, bool hasProgram,
               std::vector<Scored<std::filesystem::path>> programs);

  void setFilter(const QString &) override {}

protected:
  QString itemTitle(int section, int item) const override;
  QString itemSubtitle(int section, int item) const override;
  QString itemIconSource(int section, int item) const override;
  std::unique_ptr<ActionPanelState> createActionPanel(int section, int item) const override;

private:
  const RunProgramItem &itemAt(int section, int item) const;

  std::vector<std::string> m_cmdline;
  std::vector<Scored<std::filesystem::path>> m_programs;
  bool m_hasProgram = false;
  DefaultAction m_defaultAction = DefaultAction::RunInTerminal;

  std::vector<RunProgramItem> m_flatItems;
  int m_cmdCount = 0;
  int m_progCount = 0;
};
