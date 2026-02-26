#pragma once
#include "bridge-view.hpp"

class LocalStorageNamespaceModel;
class LocalStorageItemModel;

class LocalStorageViewHost : public ViewHostBase {
  Q_OBJECT
  Q_PROPERTY(QObject *listModel READ listModel CONSTANT)

public:
  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() override;
  void initialize() override;
  void loadInitialData() override;
  void textChanged(const QString &text) override;
  void onReactivated() override;

  QObject *listModel() const;

private:
  LocalStorageNamespaceModel *m_model = nullptr;
};

class LocalStorageItemViewHost : public ViewHostBase {
  Q_OBJECT
  Q_PROPERTY(QObject *listModel READ listModel CONSTANT)

public:
  LocalStorageItemViewHost(const QString &ns, std::vector<QString> keys);

  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() override;
  void initialize() override;
  void loadInitialData() override;
  void textChanged(const QString &text) override;
  void onReactivated() override;

  QObject *listModel() const;

private:
  QString m_ns;
  std::vector<QString> m_keys;
  LocalStorageItemModel *m_model = nullptr;
};
