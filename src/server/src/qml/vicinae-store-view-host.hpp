#pragma once
#include "section-list-model.hpp"
#include "vicinae-store-model.hpp"
#include "bridge-view.hpp"
#include "services/extension-store/vicinae-store.hpp"
#include <QFutureWatcher>

class VicinaeStoreViewHost : public ViewHostBase {
  Q_OBJECT
  Q_PROPERTY(QObject *listModel READ listModel CONSTANT)

signals:

public:
  VicinaeStoreViewHost();

  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() override;
  void initialize() override;
  void loadInitialData() override;
  void textChanged(const QString &text) override;
  void onReactivated() override;

  QObject *listModel() const { return const_cast<SectionListModel *>(&m_model); }

private:
  void fetchExtensions();
  void handleFinished();
  void refresh();

  SectionListModel m_model{this};
  VicinaeStoreSection m_section;
  VicinaeStoreService *m_store = nullptr;
  QFutureWatcher<VicinaeStore::ListResult> m_watcher;
};
