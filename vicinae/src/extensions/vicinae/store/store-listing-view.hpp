#pragma once
#include "services/extension-store/vicinae-store.hpp"
#include "ui/views/list-view.hpp"
#include <qboxlayout.h>
#include <qnamespace.h>
#include <qwidget.h>
#include <QFutureWatcher>

class VicinaeStoreListingView : public ListView {
public:
  void initialize() override;
  VicinaeStoreListingView();

private:
  void refresh();
  void handleDebounce();
  void textChanged(const QString &text) override;
  void handleFinishedQuery();
  void handleFinishedPage();

  VicinaeStoreService *m_store = nullptr;
  QFutureWatcher<VicinaeStore::ListResult> m_listResultWatcher;
  QFutureWatcher<VicinaeStore::ListResult> m_queryResultWatcher;
  QString lastQueryText;
  QTimer m_debounce;
};
