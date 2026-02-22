#include "system-run-model.hpp"
#include "actions/app/app-actions.hpp"
#include "clipboard-actions.hpp"
#include "service-registry.hpp"
#include "utils/utils.hpp"

SystemRunModel::DefaultAction SystemRunModel::parseDefaultAction(QStringView s) {
  if (s == u"run-in-terminal") return DefaultAction::RunInTerminal;
  if (s == u"run-in-terminal-hold") return DefaultAction::RunInTerminalHold;
  return DefaultAction::Run;
}

void SystemRunModel::setData(std::vector<std::string> cmdline, bool hasProgram,
                                std::vector<Scored<std::filesystem::path>> programs) {
  m_cmdline = std::move(cmdline);
  m_hasProgram = hasProgram;
  m_programs = std::move(programs);

  m_cmdCount = m_hasProgram ? 1 : 0;
  m_progCount = static_cast<int>(m_programs.size());

  std::vector<SectionInfo> sections;
  if (m_cmdCount > 0)
    sections.push_back({.name = QStringLiteral("Execute query"), .count = m_cmdCount});
  if (m_progCount > 0)
    sections.push_back(
        {.name = QStringLiteral("Programs (%1)").arg(m_progCount), .count = m_progCount});

  setSections(sections);
}

const RunProgramItem &SystemRunModel::itemAt(int section, int item) const {
  // Section 0 is cmdline if present, otherwise programs
  // Section 1 is always programs (only exists if cmdline section exists)
  static thread_local RunProgramItem cached;

  bool isCmdSection = (m_cmdCount > 0 && section == 0);
  if (isCmdSection) {
    cached = m_cmdline;
    return cached;
  }

  cached = m_programs.at(item).data.string();
  return cached;
}

QString SystemRunModel::itemTitle(int section, int item) const {
  const auto &entry = itemAt(section, item);
  return std::visit(
      overloads{[](const std::filesystem::path &path) { return QString::fromStdString(path.filename().string()); },
                [](const CommandLine &cmdline) {
                  auto query = cmdline | std::views::join_with(' ') | std::ranges::to<std::string>();
                  return QString::fromStdString(query);
                }},
      entry);
}

QString SystemRunModel::itemSubtitle(int section, int item) const {
  const auto &entry = itemAt(section, item);
  return std::visit(
      overloads{[](const std::filesystem::path &path) {
                  return QString::fromStdString(compressPath(path).string());
                },
                [](const CommandLine &) { return QString(); }},
      entry);
}

QString SystemRunModel::itemIconSource(int section, int item) const {
  const auto &entry = itemAt(section, item);
  return std::visit(
      overloads{[](const std::filesystem::path &) { return QString(); },
                [this](const CommandLine &) { return imageSourceFor(ImageURL::builtin("terminal")); }},
      entry);
}

std::unique_ptr<ActionPanelState> SystemRunModel::createActionPanel(int section, int item) const {
  const auto &entry = itemAt(section, item);
  auto panel = std::make_unique<ListActionPanelState>();
  auto *sec = panel->createSection();

  auto appDb = scope().services()->appDb();
  auto terminal = appDb->terminalEmulator();

  auto createTerminalActions = [&](std::vector<QString> args) {
    if (!terminal) return;

    auto hold = new OpenInTerminalAction(terminal, args);
    auto noHold = new OpenInTerminalAction(terminal, args, {.hold = false});
    auto runRaw = new OpenRawProgramAction(args);

    hold->setTitle(QString("Open in %1 (hold)").arg(terminal->displayName()));
    noHold->setTitle(QString("Open in %1").arg(terminal->displayName()));

    std::array<AbstractAction *, 3> actions = {hold, noHold, runRaw};

    switch (m_defaultAction) {
    case DefaultAction::RunInTerminal:
      std::iter_swap(actions.begin(), std::ranges::find(actions, noHold));
      break;
    case DefaultAction::Run:
      std::iter_swap(actions.begin(), std::ranges::find(actions, runRaw));
      break;
    default:
      break;
    }

    for (auto action : actions) sec->addAction(action);
  };

  std::visit(
      overloads{[&](const std::filesystem::path &path) {
                  createTerminalActions({QString::fromStdString(path.string())});
                  sec->addAction(new CopyToClipboardAction(
                      Clipboard::Text(QString::fromStdString(path.string())), "Copy exec path"));
                },
                [&](const CommandLine &cmdline) { createTerminalActions(Utils::toQStringVec(cmdline)); }},
      entry);

  return panel;
}
