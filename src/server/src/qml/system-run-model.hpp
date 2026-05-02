#pragma once
#include "fuzzy/scored.hpp"
#include "section-source.hpp"
#include <filesystem>
#include <vector>

enum class SystemRunDefaultAction { RunInTerminal, RunInTerminalHold, Run };

SystemRunDefaultAction parseSystemRunDefaultAction(QStringView s);

class CommandLineSection : public SectionSource {
public:
  void setCommandLine(std::vector<std::string> cmdline, bool hasProgram);

  void setDefaultAction(SystemRunDefaultAction action) { m_defaultAction = action; }

  QString sectionName() const override { return QStringLiteral("Execute query"); }
  int count() const override { return m_hasProgram ? 1 : 0; }

protected:
  QString itemTitle(int i) const override;
  QString itemIconSource(int i) const override;
  std::unique_ptr<ActionPanelState> actionPanel(int i) const override;

private:
  std::vector<std::string> m_cmdline;
  bool m_hasProgram = false;
  SystemRunDefaultAction m_defaultAction = SystemRunDefaultAction::RunInTerminal;
};

class ProgramsSection : public SectionSource {
public:
  void setPrograms(std::vector<Scored<std::filesystem::path>> programs);

  void setDefaultAction(SystemRunDefaultAction action) { m_defaultAction = action; }

  QString sectionName() const override { return QStringLiteral("Programs (%1)").arg(m_programs.size()); }

  int count() const override { return static_cast<int>(m_programs.size()); }

protected:
  QString itemTitle(int i) const override;
  QString itemSubtitle(int i) const override;
  QString itemIconSource(int) const override { return {}; }
  std::unique_ptr<ActionPanelState> actionPanel(int i) const override;

private:
  std::vector<Scored<std::filesystem::path>> m_programs;
  SystemRunDefaultAction m_defaultAction = SystemRunDefaultAction::RunInTerminal;
};
