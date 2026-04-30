#pragma once
#include "bridge-view.hpp"
#include "services/extension-store/vicinae-store.hpp"
#include <QFutureWatcher>

class VicinaeStoreModel;

class VicinaeStoreViewHost : public ViewHostBase {
  Q_OBJECT

signals:

public:
  Q_PROPERTY(QObject *listModel READ listModel CONSTANT)

  VicinaeStoreViewHost();

  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() override;
  void initialize() override;
  void loadInitialData() override;
  void textChanged(const QString &text) override;
  void onReactivated() override;

  QObject *listModel() const;

private:
  void fetchExtensions();
  void handleFinished();
  void refresh();

  VicinaeStoreModel *m_model = nullptr;
  VicinaeStoreService *m_store = nullptr;
  QFutureWatcher<VicinaeStore::ListResult> m_watcher;
};
