#pragma once
#include "qml-bridge-view.hpp"
#include "root-search/extensions/extension-root-provider.hpp"

class QmlProviderSearchModel;

class QmlProviderSearchViewHost : public QmlBridgeViewBase {
  Q_OBJECT
  Q_PROPERTY(QObject *listModel READ listModel CONSTANT)

public:
  explicit QmlProviderSearchViewHost(const ExtensionRootProvider &provider);

  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() const override;
  void initialize() override;
  void loadInitialData() override;
  void textChanged(const QString &text) override;
  void onReactivated() override;

  QObject *listModel() const;

private:
  void refresh(const QString &text);

  QString m_providerId;
  QString m_displayName;
  ImageURL m_icon;
  QmlProviderSearchModel *m_model = nullptr;
};
