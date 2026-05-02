#pragma once
#include "browse-apps-model.hpp"
#include "list-view-host.hpp"

class BrowseAppsViewHost : public ListViewHost {
  Q_OBJECT

public:
  void initialize() override;
  void loadInitialData() override;

private:
  void reload();

  BrowseAppsSection m_section;
};
