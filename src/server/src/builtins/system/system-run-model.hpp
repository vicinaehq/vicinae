#pragma once
#include <glaze/core/meta.hpp>
#include <QCoreApplication>
#include "command/preference-schema.hpp"
#include "fuzzy/scored.hpp"
#include "ui/views/section-source.hpp"
#include <QCoreApplication>
#include <filesystem>
#include <vector>

enum class SystemRunDefaultAction { RunInTerminal, RunInTerminalHold, Run };

template <> struct glz::meta<SystemRunDefaultAction> {
  using enum SystemRunDefaultAction;
  static constexpr auto value =
      glz::enumerate("run-in-terminal", RunInTerminal, "run-in-terminal-hold", RunInTerminalHold, "run", Run);
};

struct SystemRunPreferences {
  SystemRunDefaultAction defaultAction = SystemRunDefaultAction::RunInTerminal;
};

template <> struct PreferenceSchema<SystemRunPreferences> {
  PreferenceMeta defaultAction{
      .key = "default-action",
      .title = tr("Default Action"),
      .description = tr("The default action to run on pressing return"),
      .options =
          [] {
            return std::vector<Preference::DropdownData::Option>{
                option(SystemRunDefaultAction::RunInTerminal, tr("Run in terminal")),
                option(SystemRunDefaultAction::RunInTerminalHold, tr("Run in terminal (hold)")),
                option(SystemRunDefaultAction::Run, tr("Run directly")),
            };
          },
  };
  Q_DECLARE_TR_FUNCTIONS(SystemRunPreferences)
};

class CommandLineSection : public SectionSource {
  Q_DECLARE_TR_FUNCTIONS(CommandLineSection)
public:
  void setCommandLine(std::vector<std::string> cmdline, bool hasProgram);

  void setDefaultAction(SystemRunDefaultAction action) { m_defaultAction = action; }

  QString sectionName() const override { return tr("Execute query"); }
  int count() const override { return m_hasProgram ? 1 : 0; }

protected:
  QString itemTitle(int i) const override;
  std::optional<ImageURL> itemIcon(int i) const override;
  std::unique_ptr<ActionPanelState> actionPanel(int i) const override;

private:
  std::vector<std::string> m_cmdline;
  bool m_hasProgram = false;
  SystemRunDefaultAction m_defaultAction = SystemRunDefaultAction::RunInTerminal;
};

class ProgramsSection : public SectionSource {
  Q_DECLARE_TR_FUNCTIONS(ProgramsSection)
public:
  void setPrograms(std::vector<Scored<std::filesystem::path>> programs);

  void setDefaultAction(SystemRunDefaultAction action) { m_defaultAction = action; }

  QString sectionName() const override { return tr("Programs (%1)").arg(m_programs.size()); }

  int count() const override { return static_cast<int>(m_programs.size()); }

protected:
  QString itemTitle(int i) const override;
  QString itemSubtitle(int i) const override;
  std::optional<ImageURL> itemIcon(int) const override { return std::nullopt; }
  std::unique_ptr<ActionPanelState> actionPanel(int i) const override;

private:
  std::vector<Scored<std::filesystem::path>> m_programs;
  SystemRunDefaultAction m_defaultAction = SystemRunDefaultAction::RunInTerminal;
};
