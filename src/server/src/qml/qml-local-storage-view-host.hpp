#pragma once
#include "qml-bridge-view.hpp"

class QmlLocalStorageNamespaceModel;
class QmlLocalStorageItemModel;

class QmlLocalStorageViewHost : public QmlBridgeViewBase {
  Q_OBJECT
  Q_PROPERTY(QObject *listModel READ listModel CONSTANT)

public:
  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() const override;
  void initialize() override;
  void loadInitialData() override;
  void textChanged(const QString &text) override;
  void onReactivated() override;

  QObject *listModel() const;

private:
  QmlLocalStorageNamespaceModel *m_model = nullptr;
};

class QmlLocalStorageItemViewHost : public QmlBridgeViewBase {
  Q_OBJECT
  Q_PROPERTY(QObject *listModel READ listModel CONSTANT)

public:
  QmlLocalStorageItemViewHost(const QString &ns, std::vector<QString> keys);

  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() const override;
  void initialize() override;
  void loadInitialData() override;
  void textChanged(const QString &text) override;
  void onReactivated() override;

  QObject *listModel() const;

private:
  QString m_ns;
  std::vector<QString> m_keys;
  QmlLocalStorageItemModel *m_model = nullptr;
};
