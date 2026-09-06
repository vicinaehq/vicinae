#pragma once
#include "qml/oauth-token-store-model.hpp"
#include "ui/views/list-view-host.hpp"

class OAuthTokenStoreViewHost : public ListViewHost {
  Q_OBJECT

public:
  void initialize() override;
  void loadInitialData() override;

private:
  void reload();

  OAuthTokenStoreSection m_section;
};
