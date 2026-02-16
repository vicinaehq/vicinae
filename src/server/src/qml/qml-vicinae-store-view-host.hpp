#pragma once
#include "qml-bridge-view.hpp"
#include "services/extension-store/vicinae-store.hpp"
#include <QFutureWatcher>
#include <QTimer>

class QmlVicinaeStoreModel;

class QmlVicinaeStoreViewHost : public QmlBridgeViewBase {
  Q_OBJECT

signals:

public:
  Q_PROPERTY(QObject *listModel READ listModel CONSTANT)

  QmlVicinaeStoreViewHost();

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

  QmlVicinaeStoreModel *m_model = nullptr;
  VicinaeStoreService *m_store = nullptr;
  QFutureWatcher<VicinaeStore::ListResult> m_listResultWatcher;
  QFutureWatcher<VicinaeStore::ListResult> m_queryResultWatcher;
  QString m_lastQueryText;
  QTimer m_debounce;
};
