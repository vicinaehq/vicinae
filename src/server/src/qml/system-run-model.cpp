#include "system-run-model.hpp"
#include "actions/app/app-actions.hpp"
#include "clipboard-actions.hpp"
#include "service-registry.hpp"
#include "utils/utils.hpp"

SystemRunDefaultAction parseSystemRunDefaultAction(QStringView s) {
  if (s == u"run-in-terminal") return SystemRunDefaultAction::RunInTerminal;
  if (s == u"run-in-terminal-hold") return SystemRunDefaultAction::RunInTerminalHold;
  return SystemRunDefaultAction::Run;
}

// --- CommandLineSection ---

void CommandLineSection::setCommandLine(std::vector<std::string> cmdline, bool hasProgram) {
  m_cmdline = std::move(cmdline);
  m_hasProgram = hasProgram;
  notifyChanged();
}

QString CommandLineSection::itemTitle(int) const {
  auto query = m_cmdline | std::views::join_with(' ') | std::ranges::to<std::string>();
  return QString::fromStdString(query);
}

QString CommandLineSection::itemIconSource(int) const {
  return imageSourceFor(ImageURL::builtin("terminal"));
}

std::unique_ptr<ActionPanelState> CommandLineSection::actionPanel(int) const {
  auto panel = std::make_unique<ListActionPanelState>();
  auto *sec = panel->createSection();

  auto appDb = scope().services()->appDb();
  auto terminal = appDb->terminalEmulator();

  if (terminal) {
    auto args = Utils::toQStringVec(m_cmdline);

    auto hold = new OpenInTerminalAction(terminal, args);
    auto noHold = new OpenInTerminalAction(terminal, args, {.hold = false});
    auto runRaw = new OpenRawProgramAction(args);

    hold->setTitle(QString("Open in %1 (hold)").arg(terminal->displayName()));
    noHold->setTitle(QString("Open in %1").arg(terminal->displayName()));

    std::array<AbstractAction *, 3> actions = {hold, noHold, runRaw};

    switch (m_defaultAction) {
    case SystemRunDefaultAction::RunInTerminal:
      std::iter_swap(actions.begin(), std::ranges::find(actions, noHold));
      break;
    case SystemRunDefaultAction::Run:
      std::iter_swap(actions.begin(), std::ranges::find(actions, runRaw));
      break;
    default:
      break;
    }

    for (auto action : actions)
      sec->addAction(action);
  }

  return panel;
}

// --- ProgramsSection ---

void ProgramsSection::setPrograms(std::vector<Scored<std::filesystem::path>> programs) {
  m_programs = std::move(programs);
  notifyChanged();
}

QString ProgramsSection::itemTitle(int i) const {
  return QString::fromStdString(m_programs.at(i).data.filename().string());
}

QString ProgramsSection::itemSubtitle(int i) const {
  return QString::fromStdString(compressPath(m_programs.at(i).data).string());
}

std::unique_ptr<ActionPanelState> ProgramsSection::actionPanel(int i) const {
  auto panel = std::make_unique<ListActionPanelState>();
  auto *sec = panel->createSection();

  auto appDb = scope().services()->appDb();
  auto terminal = appDb->terminalEmulator();
  const auto &path = m_programs.at(i).data;

  if (terminal) {
    std::vector<QString> args = {QString::fromStdString(path.string())};

    auto hold = new OpenInTerminalAction(terminal, args);
    auto noHold = new OpenInTerminalAction(terminal, args, {.hold = false});
    auto runRaw = new OpenRawProgramAction(args);

    hold->setTitle(QString("Open in %1 (hold)").arg(terminal->displayName()));
    noHold->setTitle(QString("Open in %1").arg(terminal->displayName()));

    std::array<AbstractAction *, 3> actions = {hold, noHold, runRaw};

    switch (m_defaultAction) {
    case SystemRunDefaultAction::RunInTerminal:
      std::iter_swap(actions.begin(), std::ranges::find(actions, noHold));
      break;
    case SystemRunDefaultAction::Run:
      std::iter_swap(actions.begin(), std::ranges::find(actions, runRaw));
      break;
    default:
      break;
    }

    for (auto action : actions)
      sec->addAction(action);
  }

  sec->addAction(
      new CopyToClipboardAction(Clipboard::Text(QString::fromStdString(path.string())), "Copy exec path"));

  return panel;
}
