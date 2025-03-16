#pragma once
#include "ui/form/base-input.hpp"
#include "ui/omni-list.hpp"
#include "ui/popover.hpp"
#include <qlineedit.h>
#include <qobject.h>
#include <qtmetamacros.h>

class SelectorInput : public QWidget {
public:
  class AbstractItem : public AbstractDefaultListItem {
  public:
    AbstractItem() {}

    virtual OmniIconUrl icon() const = 0;
    virtual QString displayName() const = 0;
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

private:
  Q_OBJECT

  int POPOVER_HEIGHT = 300;

protected:
  OmniList *list;
  BaseInput *inputField;
  QLineEdit *searchField;
  OmniIcon *collapseIcon;
  OmniIcon *selectionIcon;
  Popover *popover;
  QString selectedId;

  bool eventFilter(QObject *obj, QEvent *event) override;

  using UpdateItemCallback = std::function<void(AbstractItem *item)>;

  struct UpdateItemPayload {
    QString icon;
    QString displayName;
  };

public:
  SelectorInput(const QString &name = "");
  ~SelectorInput();

  void beginUpdate();
  void commitUpdate();
  void clear();
  void addItem(std::unique_ptr<AbstractItem> item);
  void addSection(const QString &name);

  void updateItem(const QString &id, const UpdateItemCallback &cb);
  const AbstractItem *value() const;
  void setValue(const QString &id);
  QString searchText();

signals:
  void textChanged(const QString &s);
  void selectionChanged(const AbstractItem &item);

private slots:
  void handleTextChanged(const QString &text);
  void itemActivated(const OmniList::AbstractVirtualItem &vitem);
  void itemUpdated(const OmniList::AbstractVirtualItem &item);
  void showPopover();
};
