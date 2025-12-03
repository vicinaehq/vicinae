#pragma once
#include "actions/app/app-actions.hpp"
#include "program-db/program-db.hpp"
#include "service-registry.hpp"
#include "services/app-service/app-service.hpp"
#include "ui/views/typed-list-view.hpp"
#include "ui/vlist/common/vertical-list-model.hpp"
#include "utils/utils.hpp"
#include "xdgpp/desktop-entry/exec.hpp"

using CommandLine = std::vector<std::string>;

using RunProgramItem = std::variant<CommandLine, std::filesystem::path>;

enum class RunProgramSection { CommandLine, AvailablePrograms };

class RunProgramListModel : public vicinae::ui::VerticalListModel<RunProgramItem, RunProgramSection> {
public:
  RunProgramListModel() {}

  RunProgramItem sectionItemAt(RunProgramSection id, int itemIdx) const override {
    switch (id) {
    case RunProgramSection::AvailablePrograms:
      return m_progs.at(itemIdx).data.string();
    case RunProgramSection::CommandLine:
      return m_commandLine;
    default:
      return "";
    }
  }

  ItemData createItemData(const Item &item) const override {
    const auto visitor =
        overloads{[](const std::filesystem::path &path) {
                    return ItemData{.title = path.filename().c_str(), .subtitle = compressPath(path).c_str()};
                  },
                  [](const CommandLine &cmdline) {
                    auto query = cmdline | std::views::join_with(' ') | std::ranges::to<std::string>();
                    return ItemData{.title = query.c_str(), .icon = ImageURL::builtin("terminal")};
                  }};
    return std::visit(visitor, item);
  }

  std::string_view sectionName(RunProgramSection id) const override {
    switch (id) {
    case RunProgramSection::AvailablePrograms:
      return "Programs ({count})";
    case RunProgramSection::CommandLine:
      return "Execute query";
    default:
      return "Invalid executable";
    }
  }

  int sectionItemCount(RunProgramSection id) const override {
    switch (id) {
    case RunProgramSection::AvailablePrograms:
      return m_progs.size();
    case RunProgramSection::CommandLine:
      return m_hasProgram;
    default:
      break;
    }
  }

  int sectionCount() const override { return 2; }

  RunProgramSection sectionIdFromIndex(int idx) const override {
    static const std::array<RunProgramSection, 2> sections = {RunProgramSection::CommandLine,
                                                              RunProgramSection::AvailablePrograms};
    return sections.at(idx);
  }

  VListModel::StableID stableId(const Item &item) const override {
    const auto visitor = overloads{[](const std::filesystem::path &path) { return hash(path); },
                                   [this](const CommandLine &cmdline) { return randomId(); }};
    return std::visit(visitor, item);
  }

  void setCommandLine(const std::vector<std::string> &cmdline, bool hasProgram) {
    m_commandLine = cmdline;
    m_hasProgram = hasProgram;
  }

  void setPrograms(const std::vector<Scored<std::filesystem::path>> &progs) { m_progs = progs; }

  void requestUpdate() { emit dataChanged(); }

private:
  std::vector<std::string> m_commandLine;
  std::vector<Scored<std::filesystem::path>> m_progs;
  bool m_hasProgram = false;
};

class SystemRunView : public TypedListView<RunProgramListModel> {
  void textChanged(const QString &text) override {
    auto str = text.trimmed().toStdString();
    auto ss = xdgpp::ExecParser("").parse(str);
    bool hasProg = false;

    if (!ss.empty()) { hasProg = ProgramDb::programPath(ss.front()).has_value(); }

    m_model->setPrograms(m_programDb.search(str, 100));
    m_model->setCommandLine(ss, hasProg);
    m_model->requestUpdate();
    m_list->selectFirst();
  }

  std::unique_ptr<ActionPanelState> createActionPanel(const ItemType &item) const override {
    auto panel = std::make_unique<ListActionPanelState>();
    auto appDb = context()->services->appDb();
    auto terminal = appDb->terminalEmulator();
    auto section = panel->createSection();
    const auto visitor =
        overloads{[&](const std::filesystem::path &path) {
                    section->addAction(new OpenInTerminalAction(terminal, {path.c_str()}));
                  },
                  [&](const CommandLine &cmdline) {
                    if (terminal) {
                      section->addAction(new OpenInTerminalAction(terminal, Utils::toQStringVec(cmdline)));
                    }
                  }};

    std::visit(visitor, item);
    return panel;
  }

  void initialize() override {
    TypedListView::initialize();
    setModel(m_model);
    setSearchPlaceholderText("Search for a program to execute...");
    connect(&m_programDb, &ProgramDb::backgroundScanFinished, this, [this]() { textChanged(searchText()); });
    m_programDb.backgroundScan();
  }

private:
  RunProgramListModel *m_model = new RunProgramListModel;
  ProgramDb m_programDb;
};
