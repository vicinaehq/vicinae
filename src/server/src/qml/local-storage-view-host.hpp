#pragma once
#include "list-view-host.hpp"
#include "local-storage-model.hpp"

class LocalStorageViewHost : public ListViewHost {
  Q_OBJECT

public:
  void initialize() override;
  void loadInitialData() override;

private:
  LocalStorageNamespaceSection m_section;
};

class LocalStorageItemViewHost : public ListViewHost {
  Q_OBJECT

public:
  LocalStorageItemViewHost(const QString &ns, std::vector<QString> keys);

  void initialize() override;
  void loadInitialData() override;

private:
  QString m_ns;
  std::vector<QString> m_keys;
  LocalStorageItemSection m_section;
};
