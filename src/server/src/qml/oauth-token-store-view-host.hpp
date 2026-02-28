#pragma once
#include "bridge-view.hpp"

class OAuthTokenStoreModel;

class OAuthTokenStoreViewHost : public ViewHostBase {
  Q_OBJECT
  Q_PROPERTY(QObject *listModel READ listModel CONSTANT)

public:
  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() override;
  void initialize() override;
  void loadInitialData() override;
  void textChanged(const QString &text) override;
  void onReactivated() override;
  void beforePop() override;

  QObject *listModel() const;

private:
  void reload();

  OAuthTokenStoreModel *m_model = nullptr;
};
