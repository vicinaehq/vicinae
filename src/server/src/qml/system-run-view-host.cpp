#include "system-run-view-host.hpp"
#include "system-run-model.hpp"
#include "xdgpp/desktop-entry/exec.hpp"

void SystemRunViewHost::initialize() {
  BaseView::initialize();
  initModel();

  auto defaultAction =
      parseSystemRunDefaultAction(command()->preferenceValues().value("default-action").toString());
  m_cmdSection.setDefaultAction(defaultAction);
  m_progSection.setDefaultAction(defaultAction);

  model()->addSource(&m_cmdSection);
  model()->addSource(&m_progSection);

  setSearchPlaceholderText("Search for a program to execute...");
  setLoading(true);

  connect(&m_programDb, &ProgramDb::backgroundScanFinished, this, [this]() {
    setLoading(false);
    refresh(searchText());
  });
  m_programDb.backgroundScan();
}

void SystemRunViewHost::loadInitialData() {}

void SystemRunViewHost::textChanged(const QString &text) { refresh(text); }

void SystemRunViewHost::refresh(const QString &text) {
  auto str = text.trimmed().toStdString();
  auto parsed = xdgpp::ExecParser("").parse(str);
  bool hasProg = false;

  if (!parsed.empty()) hasProg = ProgramDb::programPath(parsed.front()).has_value();

  m_cmdSection.setCommandLine(std::move(parsed), hasProg);
  m_progSection.setPrograms(m_programDb.search(str, 100));
}
