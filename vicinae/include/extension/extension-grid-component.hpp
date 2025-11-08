#pragma once
#include <qstackedwidget.h>
#include <qtimer.h>
#include <qwidget.h>
#include "extend/grid-model.hpp"
#include "extension/extension-view.hpp"
#include "ui/empty-view/empty-view.hpp"
#include "extension/extension-grid.hpp"

class ExtensionGridComponent : public ExtensionSimpleView {
public:
  void render(const RenderModel &baseModel) override;
  void onSelectionChanged(const GridItemViewModel *item);
  void onItemActivated(const GridItemViewModel &item);
  void handleDebouncedSearchNotification();
  void textChanged(const QString &text) override;
  bool inputFilter(QKeyEvent *event) override;

  ExtensionGridComponent();

private:
  EmptyViewWidget *m_emptyView = new EmptyViewWidget(this);
  QStackedWidget *m_content = new QStackedWidget(this);
  GridModel _model;
  ExtensionGridList *m_list = new ExtensionGridList;
  bool _shouldResetSelection;
  QTimer *_debounce;
};
