#pragma once
#include "oauth-token-store-model.hpp"
#include "list-view-host.hpp"

class OAuthTokenStoreViewHost : public ListViewHost {
  Q_OBJECT

public:
  void initialize() override;
  void loadInitialData() override;

private:
  void reload();

  OAuthTokenStoreSection m_section;
};
