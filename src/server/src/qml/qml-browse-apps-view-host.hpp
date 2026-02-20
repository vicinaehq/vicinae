#pragma once
#include "qml-bridge-view.hpp"

class QmlBrowseAppsModel;

class QmlBrowseAppsViewHost : public QmlBridgeViewBase {
  Q_OBJECT
  Q_PROPERTY(QObject *listModel READ listModel CONSTANT)

public:
  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() const override;
  void initialize() override;
  void loadInitialData() override;
  void textChanged(const QString &text) override;
  void onReactivated() override;
  void beforePop() override;

  QObject *listModel() const;

private:
  void reload();

  QmlBrowseAppsModel *m_model = nullptr;
};
