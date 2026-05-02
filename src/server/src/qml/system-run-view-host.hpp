#pragma once
#include "program-db/program-db.hpp"
#include "system-run-model.hpp"
#include "list-view-host.hpp"

class SystemRunViewHost : public ListViewHost {
  Q_OBJECT

public:
  void initialize() override;
  void loadInitialData() override;
  void textChanged(const QString &text) override;

private:
  void refresh(const QString &text);

  CommandLineSection m_cmdSection;
  ProgramsSection m_progSection;
  ProgramDb m_programDb;
};
