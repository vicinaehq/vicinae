#pragma once
#include "common.hpp"
#include "ui/focus-notifier.hpp"
#include "ui/form/base-input.hpp"
#include "ui/loading-bar/horizontal-loading-bar.hpp"
#include "ui/image/image.hpp"
#include "ui/popover/popover.hpp"
#include "ui/vlist/common/dropdown-model.hpp"
#include "ui/vlist/vlist.hpp"
#include <qjsonvalue.h>
#include <qlineedit.h>
#include <qobject.h>
#include <qstackedwidget.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class SelectorInputV2 : public JsonFormItemWidget {
  Q_OBJECT

signals:
  void textChanged(const QString &s);
  void selectionChanged(const vicinae::ui::DropdownItem &item);

public:
  SelectorInputV2(QWidget *parent = nullptr);

  FocusNotifier *focusNotifier() const override;

  void setIsLoading(bool value);
  QJsonValue asJsonValue() const override;
  std::optional<vicinae::ui::DropdownItem> value() const;
  bool setValue(const QString &id);
  void setValueAsJson(const QJsonValue &value) override;
  QString searchText();
  void openSelector() { showPopover(); }
  void setModel(vicinae::ui::DropdownModel *model);

protected:
  vicinae::ui::VListWidget *m_list;
  BaseInput *inputField;
  QLineEdit *m_searchField;
  ImageWidget *collapseIcon;
  HorizontalLoadingBar *m_loadingBar = new HorizontalLoadingBar(this);
  ImageWidget *selectionIcon;
  Popover *popover;
  QStackedWidget *m_content = new QStackedWidget(popover);
  QWidget *m_emptyView = new QWidget(m_content);

  vicinae::ui::DropdownModel *m_model = nullptr;

  bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
  void handleTextChanged(const QString &text);
  void showPopover();

private:
  FocusNotifier *m_focusNotifier = new FocusNotifier(this);
  bool m_focused = false;
  bool m_defaultFilterEnabled = true;
  int POPOVER_HEIGHT = 300;

  void listHeightChanged(int height);
};
