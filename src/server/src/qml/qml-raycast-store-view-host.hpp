#pragma once
#include "qml-bridge-view.hpp"
#include "services/raycast/raycast-store.hpp"
#include <QFutureWatcher>
#include <QTimer>

class QmlRaycastStoreModel;

class QmlRaycastStoreViewHost : public QmlBridgeViewBase {
  Q_OBJECT

signals:

public:
  Q_PROPERTY(QObject *listModel READ listModel CONSTANT)

  QmlRaycastStoreViewHost();

  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() const override;
  void initialize() override;
  void loadInitialData() override;
  void textChanged(const QString &text) override;
  void onReactivated() override;

  QObject *listModel() const;

private:
  void fetchExtensions();
  void handleFinishedPage();
  void handleFinishedQuery();
  void handleDebounce();
  void refresh();

  QmlRaycastStoreModel *m_model = nullptr;
  RaycastStoreService *m_store = nullptr;
  QFutureWatcher<Raycast::ListResult> m_listResultWatcher;
  QFutureWatcher<Raycast::ListResult> m_queryResultWatcher;
  QString m_lastQueryText;
  QTimer m_debounce;
};
