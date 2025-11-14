#pragma once
#include <qdebug.h>
#include <QJsonArray>
#include <qboxlayout.h>
#include <qevent.h>
#include <qnamespace.h>
#include <qresource.h>
#include <qstackedwidget.h>
#include <qtimer.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include "extend/list-model.hpp"
#include "extension/extension-list-detail.hpp"
#include "extension/extension-list.hpp"
#include "extension/extension-view.hpp"
#include "ui/empty-view/empty-view.hpp"
#include "ui/form/selector-input.hpp"
#include "ui/split-detail/split-detail.hpp"

class ExtensionListComponent : public ExtensionSimpleView {
public:
  void render(const RenderModel &baseModel) override;
  void onSelectionChanged(const ListItemViewModel *next);
  void onItemActivated(const ListItemViewModel &item);
  void handleDebouncedSearchNotification();
  void textChanged(const QString &text) override;
  void initialize() override;

  ExtensionListComponent();
  ~ExtensionListComponent();

private:
  void renderDropdown(const DropdownModel &dropdown);
  void handleDropdownSelectionChanged(const SelectorInput::AbstractItem &item);
  void handleDropdownSearchChanged(const QString &text);

  QWidget *searchBarAccessory() const override { return m_selector; }
  bool inputFilter(QKeyEvent *event) override;

  QStackedWidget *m_content = new QStackedWidget(this);
  EmptyViewWidget *m_emptyView = new EmptyViewWidget(this);
  SelectorInput *m_selector = new SelectorInput(this);
  SplitDetailWidget *m_split = new SplitDetailWidget(this);
  ExtensionListDetail *m_detail = new ExtensionListDetail;
  ListModel _model;
  ExtensionList *m_list = new ExtensionList;
  bool _shouldResetSelection;
  QTimer *_debounce;
  QTimer *m_dropdownDebounce = new QTimer(this);
  bool m_dropdownShouldResetSelection = false;
  int m_renderCount = 0;
  std::optional<QString> m_onSelectionChanged;
};
