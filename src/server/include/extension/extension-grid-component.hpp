#pragma once
#include <qstackedwidget.h>
#include <qtimer.h>
#include <qwidget.h>
#include "extend/grid-model.hpp"
#include "extension/extension-view.hpp"
#include "ui/empty-view/empty-view.hpp"
#include "ui/form/selector-input.hpp"
#include "extension/extension-grid.hpp"

class ExtensionGridComponent : public ExtensionSimpleView {
public:
  void render(const RenderModel &baseModel) override;
  void onSelectionChanged(const GridItemViewModel *item);
  void onItemActivated(const GridItemViewModel &item);
  void handleDebouncedSearchNotification();
  void textChanged(const QString &text) override;
  bool inputFilter(QKeyEvent *event) override;
  void initialize() override;

  ExtensionGridComponent();

private:
  void renderDropdown(const DropdownModel &dropdown);
  void handleDropdownSelectionChanged(const SelectorInput::AbstractItem &item);
  void handleDropdownSearchChanged(const QString &text);

  QWidget *searchBarAccessory() const override { return m_selector; }

private:
  EmptyViewWidget *m_emptyView = new EmptyViewWidget(this);
  QStackedWidget *m_content = new QStackedWidget(this);
  SelectorInput *m_selector = new SelectorInput(this);
  GridModel _model;
  ExtensionGridList *m_list = new ExtensionGridList;
  bool _shouldResetSelection;
  QTimer *_debounce;
  QTimer *m_dropdownDebounce = new QTimer(this);
  bool m_dropdownShouldResetSelection = false;
};
