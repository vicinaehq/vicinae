#pragma once
#include "installed-extensions-model.hpp"
#include "list-view-host.hpp"

class InstalledExtensionsViewHost : public ListViewHost {
  Q_OBJECT

public:
  void initialize() override;
  void loadInitialData() override;

private:
  void reload();

  InstalledExtensionsSection m_section;
};
