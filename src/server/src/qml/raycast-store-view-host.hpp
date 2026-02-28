#pragma once
#include "bridge-view.hpp"
#include "services/raycast/raycast-store.hpp"
#include <QFutureWatcher>
#include <QTimer>

class RaycastStoreModel;

class RaycastStoreViewHost : public ViewHostBase {
  Q_OBJECT

signals:

public:
  Q_PROPERTY(QObject *listModel READ listModel CONSTANT)

  RaycastStoreViewHost();

  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() override;
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

  RaycastStoreModel *m_model = nullptr;
  RaycastStoreService *m_store = nullptr;
  QFutureWatcher<Raycast::ListResult> m_listResultWatcher;
  QFutureWatcher<Raycast::ListResult> m_queryResultWatcher;
  QString m_lastQueryText;
  QTimer m_debounce;
};
