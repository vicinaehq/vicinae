#pragma once
#include "provider-search-model.hpp"
#include "list-view-host.hpp"
#include "root-search/extensions/extension-root-provider.hpp"

class ProviderSearchViewHost : public ListViewHost {
  Q_OBJECT

public:
  explicit ProviderSearchViewHost(const RootProvider &provider);

  void initialize() override;
  void loadInitialData() override;
  void textChanged(const QString &text) override;

private:
  void refresh(const QString &text);

  QString m_providerId;
  QString m_displayName;
  ImageURL m_icon;
  ProviderSearchSection m_section;
};
