#pragma once
#include "actions/extension/extension-actions.hpp"
#include "services/raycast/raycast-store.hpp"
#include "ui/views/list-view.hpp"
#include <qboxlayout.h>
#include <qnamespace.h>
#include <qwidget.h>
#include <QFutureWatcher>

class RaycastStoreListingView : public ListView {
public:
  void initialize() override;
  RaycastStoreListingView();

private:
  void refresh();
  void handleDebounce();
  void textChanged(const QString &text) override;
  void handleFinishedQuery();
  void handleFinishedPage();

  RaycastStoreService *m_store = nullptr;
  QFutureWatcher<Raycast::ListResult> m_listResultWatcher;
  QFutureWatcher<Raycast::ListResult> m_queryResultWatcher;
  QString lastQueryText;
  QTimer m_debounce;
};
