#pragma once
#include "actions/app/app-actions.hpp"
#include "program-db/program-db.hpp"
#include "ui/views/list-view.hpp"
#include <qlogging.h>
#include "service-registry.hpp"
#include "services/app-service/app-service.hpp"
#include "utils/utils.hpp"

class SystemRunListItem : public AbstractDefaultListItem, public ListView::Actionnable {
  std::filesystem::path m_path;

public:
  QString generateId() const override { return m_path.c_str(); }
  ItemData data() const override {
    return {.iconUrl = ImageURL::builtin("terminal"),
            .name = m_path.filename().c_str(),
            .subtitle = compressPath(m_path)};
  }

  std::unique_ptr<ActionPanelState> newActionPanel(ApplicationContext *ctx) const override {
    auto panel = std::make_unique<ActionPanelState>();
    auto section = panel->createSection();
    auto appDb = ctx->services->appDb();

    auto open = new OpenRawProgramAction(m_path.c_str(), {});

    section->addAction(open);
    open->setPrimary(true);
    open->setShortcut(KeyboardShortcutModel::enter());
    open->setClearSearch(true);

    if (auto app = appDb->terminalEmulator()) {
      auto openInTerminal = new OpenAppAction(app, "Open in terminal", {"-e", m_path.c_str()});
      openInTerminal->setShortcut(KeyboardShortcutModel::submit());
      section->addAction(openInTerminal);
    }

    if (auto app = appDb->fileBrowser()) {
      auto openLocation = new OpenAppAction(app, "Open location", {m_path.c_str()});

      openLocation->setShortcut(KeyboardShortcutModel::open());
      section->addAction(openLocation);
    }

    return panel;
  }

  QString navigationTitle() const override { return m_path.filename().c_str(); }

  SystemRunListItem(const std::filesystem::path &path) : m_path(path) {}
};

class SystemRunView : public ListView {
  void generateRootSearch() {
    m_list->updateModel([&]() {
      auto results = m_programDb.programs();
      auto &section = m_list->addSection(QString("Programs (%1)").arg(results.size()));

      for (const auto &prog : results) {
        section.addItem(std::make_unique<SystemRunListItem>(prog));
      }
    });
  }

  void textChanged(const QString &text) override {
    if (text.isEmpty()) { return generateRootSearch(); }

    m_list->updateModel([&]() {
      auto results = m_programDb.search(text);
      auto &section = m_list->addSection(QString("Results (%1)").arg(results.size()));

      for (const auto &prog : results) {
        section.addItem(std::make_unique<SystemRunListItem>(prog));
      }
    });
  }

  void initialize() override {
    setSearchPlaceholderText("Search for a program to execute...");
    connect(&m_programDb, &ProgramDb::backgroundScanFinished, this,
            [this]() { setSearchText(searchText()); });
    m_programDb.backgroundScan();
  }

private:
  ProgramDb m_programDb;
};
