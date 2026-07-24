#include "system-run-view-host.hpp"
#include "system-run-model.hpp"
#include <sstream>
#ifndef Q_OS_WIN
#include "xdgpp/desktop-entry/exec.hpp"
#endif

void SystemRunViewHost::initialize() {
  BaseView::initialize();
  initModel();

  auto defaultAction =
      parseSystemRunDefaultAction(command()->preferenceValues().value("default-action").toString());
  m_cmdSection.setDefaultAction(defaultAction);
  m_progSection.setDefaultAction(defaultAction);

  model()->addSource(&m_cmdSection);
  model()->addSource(&m_progSection);

  setSearchPlaceholderText(tr("Search for a program to execute..."));
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
#ifdef Q_OS_WIN
  std::vector<std::string> parsed;
  std::istringstream iss(str);
  for (std::string tok; iss >> tok;) {
    parsed.emplace_back(std::move(tok));
  }
#else
  auto parsed = xdgpp::ExecParser("").parse(str);
#endif
  bool hasProg = false;

  if (!parsed.empty()) hasProg = ProgramDb::programPath(parsed.front()).has_value();

  m_cmdSection.setCommandLine(std::move(parsed), hasProg);
  m_progSection.setPrograms(m_programDb.search(str, 100));
}
