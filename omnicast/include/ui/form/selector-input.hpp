#pragma once
#include "common.hpp"
#include "omni-icon.hpp"
#include "ui/focus-notifier.hpp"
#include "ui/form/base-input.hpp"
#include "ui/horizontal-loading-bar.hpp"
#include "ui/omni-list.hpp"
#include "ui/popover.hpp"
#include <memory>
#include <qjsonvalue.h>
#include <qlineedit.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class SelectorInput : public QWidget, public IJsonFormField {
public:
  class AbstractItem : public AbstractDefaultListItem {
  public:
    AbstractItem() {}

    virtual OmniIconUrl icon() const { return BuiltinOmniIconUrl("circle"); };
    virtual QString displayName() const = 0;
    virtual bool hasPartialUpdates() const override { return true; }

    /**
     * Once an item is selected a copy of it is stored as the current selection.
     * This is required to maintain an always correct selection even if the list
     * of available options changed (in case the list of options is dynamically generated
     * for instance)
     */
    virtual AbstractItem *clone() const = 0;

    ItemData data() const override { return {.iconUrl = icon(), .name = displayName()}; }
  };

  class ItemFilter : public OmniList::AbstractItemFilter {
    QString query;

    bool matches(const OmniList::AbstractVirtualItem &item) override {
      auto &dropdowmItem = static_cast<const SelectorInput::AbstractItem &>(item);

      return dropdowmItem.displayName().contains(query, Qt::CaseInsensitive);
    }

  public:
    ItemFilter(const QString &query) : query(query) {}
  };

  QJsonValue asJsonValue() const override;

private:
  Q_OBJECT

  FocusNotifier *m_focusNotifier = new FocusNotifier(this);
  bool m_focused = false;
  bool m_defaultFilterEnabled = true;
  int POPOVER_HEIGHT = 300;

protected:
  OmniList *m_list;
  BaseInput *inputField;
  QLineEdit *m_searchField;
  OmniIcon *collapseIcon;
  HorizontalLoadingBar *m_loadingBar = new HorizontalLoadingBar(this);
  OmniIcon *selectionIcon;
  Popover *popover;
  std::unique_ptr<AbstractItem> _currentSelection;

  bool eventFilter(QObject *obj, QEvent *event) override;

  using UpdateItemCallback = std::function<void(AbstractItem *item)>;

  struct UpdateItemPayload {
    QString icon;
    QString displayName;
  };

public:
  SelectorInput(QWidget *parent = nullptr);
  ~SelectorInput();

  FocusNotifier *focusNotifier() const;
  void beginUpdate();
  void commitUpdate();
  void setIsLoading(bool value);
  void clear();
  void addItem(std::unique_ptr<AbstractItem> item);
  void addSection(const QString &name);
  void clearFilter() const;
  OmniList *list() const { return m_list; }

  void updateItem(const QString &id, const UpdateItemCallback &cb);
  const AbstractItem *value() const;
  void setValue(const QString &id);
  void setValueAsJson(const QJsonValue &value) override;
  QString searchText();
  void setEnableDefaultFilter(bool value);

signals:
  void textChanged(const QString &s);
  void selectionChanged(const AbstractItem &item);

private slots:
  void handleTextChanged(const QString &text);
  void itemActivated(const OmniList::AbstractVirtualItem &vitem);
  void itemUpdated(const OmniList::AbstractVirtualItem &item);
  void showPopover();
};
